from asyncio.windows_events import NULL
import datetime as datetime
import pickle
from operator import mod
import os
import csv
from collections import Counter
import networkx as nx
import pandas as pd
import copy
#from pyvis.network import Network
#import pygexf

from numpy import append, zeros

SMOOTHCONTACTS = datetime.timedelta(seconds = 20)
def read_putty_log(filename,infectdata, contacts,generaldata): 
    # Setup a dictionary to collect infection data  
    
    with open(filename, 'r') as input:
        # Cycle through all lines in the log
        for line in input:
            # Check for relevant lines
            if line.find('Device ID')>=0 or line.find('General')>=0 or line.find('Infect')>=0 or line.find('Netz')>=0:
                # Split of and evalute the timestamp
                fields = line.split('] ')
                tdatetime = datetime.datetime.strptime(fields[0], '[%d/%m/%y - %H:%M:%S:%f').replace(microsecond=0)
                #process the data fields
                datafields = fields[1].split(':')
                if datafields[0].strip() == 'Device ID and RSSI':
                    plfields = datafields[1].split(',')
                    tcurrent_id = int(plfields[0].strip())               
                elif datafields[0].strip() == 'General':
                    plfields = datafields[1].split(',')
                    tcurrent_id = int(plfields[0].strip())
                    tcurrent_status = int(plfields[1].strip())
                    tcurrent_timer = int(plfields[2].strip())
                    tinfrevision = tcurrent_status>>5
                    tbatt_stat = (tcurrent_status>>4) % 2
                    tinfstatus = tcurrent_status & 0x0F
                    tlastreset = tdatetime - datetime.timedelta(seconds = tcurrent_timer)
                    generaldata[tcurrent_id]['general'] = [tcurrent_timer,tdatetime,tinfstatus,tinfrevision,tbatt_stat,tlastreset]  
                elif datafields[0].strip() == 'Infect':
                    plfields = datafields[1].split(',')               
                    tcurrent_id = int(plfields[0].strip())
                    tcurrent_status = int(plfields[1].strip())
                    
                    tinf_status = tcurrent_status & 0x0F 
                    tinf_rev = tcurrent_status>>5
                    
                    tcurrent_timer = int(plfields[2].strip())
                    tsource = []

                    for i in range(0,15):
                        for j in range(0,8):
                            plfields[3+i] = plfields[3+i].strip()
                            if plfields[3+i][j] == '1':
                                tsource.append(i*8+j+1)
                    tdeltatime = tcurrent_timer - generaldata[tcurrent_id]['general'][0]
                    tinf_time =  generaldata[tcurrent_id]['general'][1] + datetime.timedelta(seconds = tdeltatime)
                    tnewentry = [tinf_rev,tcurrent_id,tinf_status,tinf_time,tsource]
                    if tcurrent_id == 242 and tinf_rev ==3 and tinf_status == 2:
                        # Debug 
                        a=1
                    if tinf_status == 2 and tinf_rev==3 and tcurrent_id==42:
                        # Correct wrong entry for revision 3
                        tinf_time = datetime.datetime(2022,8,10,8,20,0)
                    if not tnewentry in generaldata[tcurrent_id]['infect']:
                        generaldata[tcurrent_id]['infect'].append(tnewentry)
                    if not tnewentry in infectdata:
                        infectdata.append(tnewentry)
                elif datafields[0].strip() == 'Netz (ID)':
                    plfields = datafields[1].split(',')
                    tcurrent_id = int(plfields[0].strip())
                    tpartner_id = int(plfields[1].strip())
                    
                    tstart = int(plfields[2].strip())
                    tduration =int (plfields[3].strip())
                    tdeltatimestart = tstart - generaldata[tcurrent_id]['general'][0]
                    tnet_timestart =  generaldata[tcurrent_id]['general'][1] + datetime.timedelta(seconds = tdeltatimestart)
                    tnet_timeend   =  tnet_timestart + datetime.timedelta(seconds = tduration)
                    if tcurrent_id == 116 or tpartner_id == 116:
                    
                        a=1
                        pass
                    if tcurrent_id > tpartner_id:
                        tnewentry = [tpartner_id,tcurrent_id,tnet_timestart,tnet_timeend]
                    else:
                        tnewentry = [tcurrent_id,tpartner_id, tnet_timestart,tnet_timeend]
                    if not tnewentry in contacts:
                        contacts.append(tnewentry)
                else:
                    print('Unknown  Field')
                    print(datafields[0].strip())
    #print(contacts)
    return generaldata, infectdata, contacts

            

def clean_infectdata(infectdata):
    infectdata.sort(key = lambda x: (x[0],x[1],x[2],x[3]))
    i = 0
    while i < len(infectdata)-1:
        if infectdata[i][0] == infectdata[i+1][0] and infectdata[i][1] == infectdata[i+1][1] and infectdata[i][2] == infectdata[i+1][2] and infectdata[i][4] == infectdata[i+1][4] :
            infectdata.pop(i+1)
        else:
            i += 1
    return infectdata

def clean_contactdata(contactdata):
    contactdata.sort(key = lambda x: (x[0],x[1],x[2]) ) 

    i= 0
    while i < len(contactdata)-1:
        if (contactdata[i][0] == contactdata[i+1][0] ) and (contactdata[i][1] == contactdata[i+1][1] ):
            if contactdata[i+1][2] <= contactdata[i][3]:
                # This combines all intervalls. Due to rounding in received times this potentially increases all intervalls
                contactdata[i][3] = max(contactdata[i+1][2],contactdata[i][3])
                contactdata.pop(i+1)
            elif contactdata[i+1][2]-contactdata[i][3] <= SMOOTHCONTACTS:
                contactdata[i][3] = contactdata[i+1][2]
                contactdata.pop(i+1)
            else:
                i += 1
        else:
            i += 1
    #for contact in contactdata:
    #    print(contact)
    return contactdata
 
def evaluateinfects(infectdata):
    #sort infect data according to revision, time and status
    infectdata.sort(key = lambda x: (x[0],x[3],x[2],x[1]))
    #initialize temporary variables 
    revision = infectdata[0][0]
    rawdata = []
    timeseries = []
    currentstatus = [0]*124
    currentstatus.insert(0,datetime.datetime(2022,1,1,0,0,0))
    i=0
    pdfrom = []
    pdto = []
    pdattr = []
    pd_nodes_id_rev = []
    pd_nodes_status_rev = []
    pd_nodes_time_rev = []
    pd_nodes_id_all = []
    pd_nodes_status_all = []
    pd_nodes_time_all = []
    while i<len(infectdata):
        if infectdata[i][0] == revision:
            # Continue adding to current time series 
            pass
        else:
            # Last inf revision is complete. Print result
            print_inf_timeseries(revision,timeseries)
            # Last inf revisino is complete. Print graph
            pdnodesraw = list(zip(pd_nodes_id_rev,pd_nodes_status_rev,pd_nodes_time_rev))
            print_inf_nodes(pdnodesraw,revision)
            pdweight = [1]*len(pdfrom)
            pddfraw = list(zip(pdfrom,pdto,pdattr,pdweight))
            
            print_inf_edges(pddfraw, revision)
            
            # reset all variables
            revision = infectdata[i][0]
            rawdata = []
            timeseries = []
            currentstatus = [0]*124
            currentstatus.insert(0,datetime.datetime(2022,1,1,0,0,0))
            pdfrom = []
            pdto = []
            pdattr = []
            pd_nodes_id_rev = []
            pd_nodes_status_rev = []
            pd_nodes_time_rev = []
        # insert the current change in infect status in database
        # set the time
        currentstatus[0] = infectdata[i][3]
        # set the status of the ID to status
      
        currentstatus[infectdata[i][1]] = infectdata[i][2]
        # add current line to the raw time series
        rawdata.append(copy.deepcopy(currentstatus))
        # set temporary number of all status
        newtime = [0]*7
        # set time in status list
        newtime.insert(0,infectdata[i][3])
        #count all occurences of a certain status
        occurences = Counter(currentstatus)
        for j in range(0,7):
            newtime[j+1] = occurences[j]
        # append new status to timeseries
        timeseries.append(newtime)
        
        # list status of node
        if infectdata[i][1] in pd_nodes_id_all:
            pd_nodes_status_all[pd_nodes_id_all.index(infectdata[i][1] )].append('['+str(infectdata[i][3].isoformat())+','+str(infectdata[i][2])+']')
            pd_nodes_time_all[pd_nodes_id_all.index(infectdata[i][1] )].append(infectdata[i][3].isoformat())

        else:
            pd_nodes_id_all.append(infectdata[i][1])
            pd_nodes_status_all.append(['['+str(infectdata[i][3].isoformat())+','+str(infectdata[i][2])+']'])
            pd_nodes_time_all.append([infectdata[i][3]])
                # list status of node

        if infectdata[i][1] in pd_nodes_id_rev:
            pd_nodes_status_rev[pd_nodes_id_rev.index(infectdata[i][1] )].append('['+str(infectdata[i][3].isoformat())+','+str(infectdata[i][2])+']')
            pd_nodes_time_rev[pd_nodes_id_rev.index(infectdata[i][1] )].append(infectdata[i][3].isoformat())
        else:
            pd_nodes_id_rev.append(infectdata[i][1])
            pd_nodes_status_rev.append(['['+str(infectdata[i][3].isoformat())+','+str(infectdata[i][2])+']'])
            pd_nodes_time_rev.append([infectdata[i][3]])
        #build the Graph
        #add node
  
        if infectdata[i][2] ==1:
            
            for id in infectdata[i][4]:
                pdfrom.append(id)
                pdto.append(infectdata[i][1])
                pdattr.append(infectdata[i][3])
            if infectdata[i][0] ==2:
                a= 1
                
        i += 1
    #write the final revision to file
    print_inf_timeseries(revision,timeseries)

    #debug
    with open('rev2raw.csv','w',newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        for row in rawdata:
            csvwriter.writerow(row)
        

    # write the final inf graph
    pdnodesraw = list(zip(pd_nodes_id_rev,pd_nodes_status_rev,pd_nodes_time_rev))
    print_inf_nodes(pdnodesraw,revision)

    pdnodesraw = list(zip(pd_nodes_id_all,pd_nodes_status_all,pd_nodes_time_all))
    print_inf_nodes(pdnodesraw,'all')
    pdweight = [1]*len(pdfrom)
    pddfraw = list(zip(pdfrom,pdto,pdattr,pdweight))
    print_inf_edges(pddfraw,revision)
    inf_nodes(infectdata)

    return NULL

def print_inf_timeseries(inf_revision,inf_timeseries):
    # write timeseries to csv file
    with open('infect_timeseries_full_rev'+str(inf_revision)+'.csv','w',  encoding='UTF8', newline='') as csvfile:
        print('Writing Timeseries File')
        header = ['Time', 'Status_S', 'Status_E','Status_I','Status_R','Status_V','Status_VT','Status_H']
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(header)
        for row in inf_timeseries:
            csvwriter.writerow(row)

    return

def print_inf_edges(pddfraw,inf_revision):
    if len(pddfraw)>0:
        pddf = pd.DataFrame(pddfraw,columns = ['Source','Target','Timeset','Weight'],)
        pddf['Timeset'] = pddf['Timeset'].apply(lambda x: x.isoformat())
        pddf.index.name = 'Id'
        with open('infect_egdes_rev'+str(inf_revision)+'.csv','w') as csvfile:
            pddf.to_csv(csvfile,  index=False)
    return

def inf_nodes(infectdata):

    #sort infect data according to revision, time and status
    infectdata.sort(key = lambda x: (x[1],x[3],x[2],x[0]))

    nodes_status = []
    pd_nodes = []
    pd_status = []
    pd_time = []
    id = infectdata[0][1]
    time = infectdata[0][3]
    status = infectdata[0][2]
    G = nx.Graph()
    G.add_node(id)
    i=1
    while i < len(infectdata):
        

        if infectdata[i][1] == id:
            nodes_status.append('['+str(time.isoformat())+','+str(infectdata[i][3].isoformat())+','+str(status)+']')
        else:
            
            nodes_status.append('['+str(time.isoformat())+','+datetime.datetime(2022,8,14,18,0,0).isoformat()+','+str(status)+']')
            pd_nodes.append(id)
            pd_status.append('<'+str(';'.join(nodes_status))+'>') 
            pd_time.append(datetime.datetime(2022,8,14,18,0,0).isoformat())
            id = infectdata[i][1]
            G.add_node(id)
        time = infectdata[i][3]
        status = infectdata[i][2]
        i += 1
        
    nodes_status.append('['+str(time.isoformat())+','+datetime.datetime(2022,8,14,18,0,0).isoformat()+','+str(status)+']')
    pd_nodes.append(id)
    pd_status.append('<'+str(';'.join(nodes_status))+'>') 
    pd_time.append(datetime.datetime(2022,8,14,18,0,0).isoformat())
    

    # nx.set_edge_attributes(G, 'start', df_edges[r'Start'])
    # nx.set_edge_attributes(G, 'end', df_edges[r'End'])
    nx.write_gexf( G , 'inf_gephi.gexf' )

    print_inf_nodes(list(zip(pd_nodes,pd_status,pd_time)),'all')

    return

def print_inf_nodes(pdnodes,inf_revision):
    if len(pdnodes)>0:
        pddf = pd.DataFrame(pdnodes,columns = ['Id','Status','Timeset'])
        with open('infect_nodes_rev'+str(inf_revision)+'.csv','w') as csvfile:
            pddf.to_csv(csvfile,  index=False)
    return





def evaluatecontacts(contactdata):
    #df_edges_raw = list(zip(contactdata))
    df_edges = pd.DataFrame(contactdata,columns = ['Source','Target','contact_start','contact_end'])
    df_edges['Type'] = 'undirected'
   # df_edges['Label'] = [i for i in range(1, df_edges.shape[0]+1)]
    df_edges['contact_start'] = pd.to_datetime(df_edges['contact_start'], utc=True)
    df_edges[r'Start'] = df_edges['contact_start'].apply(lambda x: x.isoformat())
    df_edges['contact_end'] = pd.to_datetime(df_edges['contact_end'], utc=True)
    df_edges[r'End'] = df_edges['contact_end'].apply(lambda x: x.isoformat())
    df_edges['Weight'] =(df_edges['contact_end'] - df_edges['contact_start']).dt.total_seconds()
    
    df_edges[r'Timeset'] = ['<['+str(start)+','+str(stop)+']>' for start,stop in zip(df_edges[r'Start'] ,df_edges[r'End'] )]
    df_edges = df_edges.loc[:, ['Source', 'Target','Timeset', 'Type', r'Start', r'End','Weight']]
    
    df_edges.to_csv('contact_edges_gephi.csv', index=False, encoding='utf-8')
    # G = nx.from_pandas_edgelist(df_edges, 'Source', 'Target', edge_attr=True)
    # nx.set_edge_attributes(G, 'start', df_edges[r'Start'])
    # nx.set_edge_attributes(G, 'end', df_edges[r'End'])
    # nx.write_gexf( G , 'contact_graph_gephi.gexf' )
    # with open('gephi.csv','w',  encoding='UTF8', newline='') as gephifile:
    #     print('Writing Gephi File')
    #     header = ['source', 'target', 'time_start','time_end']
    #     csvwriter = csv.writer(gephifile)
    #     csvwriter.writerow(header)
        
    #     for row in contactdata:

    #         csvwriter.writerow(row)
    return NULL

def printgeneral(general):
    with open('lastseen.csv','w',newline='') as csvfile:
        print('Writing General File')
        header = ['id','current_timer', 'datum', 'status_infect', 'infrevision','Batteriestatus','Last Reset']
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(header)
        for key in general:
            trow = [key]
            for el in general[key]['general']:
                trow.append(el)
            csvwriter.writerow(trow)
    return NULL



def get_files(path):
    for file in os.listdir(path):
        if os.path.isfile(os.path.join(path, file)):
            yield os.path.join(path, file)

def column(matrix, i):
    return [row[i] for row in matrix]

def flatten(xs):
    return [x for x in xs]

def special_clean(general, infectdata, contactdata ):
    ids_removed = [19,26,27,51,75]
    time_cutoff = datetime.datetime(2022,7,28,19,45,0)
    ids_latercontact = [2]
    time_cutoff_latercontact = datetime.datetime(2022,7,29,12,00,0)
    i = 0
    while i < len(infectdata):
        if infectdata[i][1] in ids_removed:
            infectdata.pop(i)
        else:
            i += 1
    i = 0
    while i < len(contactdata):
        if contactdata[i][0] in ids_removed or contactdata[i][1] in ids_removed:
            contactdata.pop(i)
        elif contactdata[i][3] < time_cutoff:
            contactdata.pop(i)
        elif contactdata[i][0] in ids_latercontact and contactdata[i][2] < time_cutoff_latercontact:
            contactdata.pop(i)
        else:
            i += 1
    for key in ids_removed:
        general.pop(key,None)
    return general, infectdata, contactdata 

def main():
    path = 'Logs'
    logfiles = get_files(path)
    savefilenameinfect = "datainfect.txt"
    savefilenamecontact = "datacontact.txt"
    savefilenamegeneral = "lastseen.txt"
    # Setup a dictionary to collect infection data  
    data = dict((el,{'general':[0,0], 'infect':[]}) for el in range(1,121))
    # setup array to collect all contacts data
    try:
        with open(savefilenamecontact,'rb') as readfile:
            contactdata = pickle.load(readfile)
    except:  
        print('Error reading contact data')
        contactdata = []

    # setup array to collect all infect data
    try:
        with open(savefilenameinfect,'rb') as readfile:
            infectdata = pickle.load(readfile)
    except:  
        print('Error reading infect data')
        infectdata = []

    # setup array to collect all general data
    try:
        with open(savefilenamegeneral,'rb') as readfile:
            generaldata = pickle.load(readfile)
    except:  
        print('Error reading general data')
        generaldata = dict((el,{'general':[0,0,0,0], 'infect':[]}) for el in range(1,123))

    for filename in logfiles:
        generaldata, infectdata, contactdata = read_putty_log(filename,infectdata, contactdata, generaldata)

    infectdata = clean_infectdata(infectdata)
    contactdata = clean_contactdata(contactdata)

    generaldata, infectdata, contactdata = special_clean(generaldata, infectdata, contactdata )
    evaluateinfects(infectdata)

    evaluatecontacts(contactdata)
    printgeneral(generaldata)
    with open(savefilenameinfect,'wb') as savefile:
        #print(infectdata)
        pickle.dump(infectdata,savefile)
    with open(savefilenamecontact,'wb') as savefile:
        pickle.dump(contactdata,savefile)
    with open(savefilenamegeneral,'wb') as savefile:
        pickle.dump(generaldata,savefile)

if __name__ == "__main__":
    main()