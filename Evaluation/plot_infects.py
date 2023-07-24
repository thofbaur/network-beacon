import matplotlib.pyplot as plt
import csv
import datetime
import pandas as pd
import networkx as nx
from matplotlib.pyplot import figure

#from networkx.drawing.nx_agraph import graphviz_layout

def plot_timeseries(infrevision,totalS,finaltime):
    time_series = []
    t_values = []
    status_s_values = []
    status_e_values = []
    status_i_values = []
    status_r_values = []
    status_se_values = []
    status_v_values = []
    status_vt_values = []
    status_h_values = []
    numberScorrection = 0
    with open('infect_timeseries_full_rev'+str(infrevision)+'.csv','r') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        line_count = 0
        for row in csv_reader:
            
            if line_count == 0:
                #print(f'Column names are {", ".join(row)}')
                pass
            else:
                rowint = [datetime.datetime.strptime(row[0], '%Y-%m-%d %H:%M:%S')]
                for i in range( 1,len(row) ):
                    rowint.append(int(row[i]))
                numberScorrection = max(numberScorrection,sum(rowint[1:])-totalS)
                if sum(rowint[2:4])>=1 and rowint[0] <= finaltime:
                    #for entry in rowint:
                    #    print(type(entry))
                    t_values.append(rowint[0])
                    status_s_values.append(rowint[1]-numberScorrection)
                    status_se_values.append(rowint[1]+rowint[2]-numberScorrection)
                    status_e_values.append(rowint[2])
                    status_i_values.append(rowint[3])
                    status_r_values.append(rowint[4])
                    status_v_values.append(rowint[5])
                    status_vt_values.append(rowint[6])
                    status_h_values.append(rowint[7])
                    time_series.append(rowint)
            line_count += 1
    fig = plt.figure()        
    plt.plot(t_values,status_se_values, 'g--', t_values, status_i_values, 'r-',t_values,status_r_values,'b-')
    plt.xlabel('Zeit')
    plt.ylabel('Anzahl')
    fig.set_size_inches(16,10)
   # plt.show()

    #fig.set_dpi(300)

    plt.savefig('plot_infect_rev'+str(infrevision)+'.png')
    infcompact = list(zip(t_values,status_se_values,status_i_values, status_r_values,status_v_values,status_vt_values,status_h_values))
    print_inf_timeseries(infrevision,infcompact)
    return

def print_inf_timeseries(inf_revision,inf_timeseries):
    # write timeseries to csv file
    with open('infect_timeseries_compact_rev'+str(inf_revision)+'.csv','w',  encoding='UTF8', newline='') as csvfile:
        print('Writing Timeseries File')
        header = ['Time', 'Status_SE','Status_I','Status_R','Status_V','Status_VT','Status_H']
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(header)
        for row in inf_timeseries:
            csvwriter.writerow(row)

    return

def plot_infecttree(filename,sourceIds):
    pddf = pd.read_csv(filename)
    
    #totaldeltatime = max(datetime.datetime.strptime(pddf['time'], '%Y-%m-%d %H:%M:%S'))-min(datetime.datetime.strptime(pddf['time'], '%Y-%m-%d %H:%M:%S'))
    G = nx.DiGraph()
    G = nx.from_pandas_edgelist(pddf, source = 'Source', target = 'Target', edge_attr = 'Timeset',create_using=nx.DiGraph())
  #  for id in sourceIds:
  #      path = nx.shortest_path_length(G, id)

    pos = hierarchy_pos(G,sourceIds)
   # nx.draw(G, pos, with_labels=True, node_color='lightblue', node_size=500)
  #  pos = graphviz_layout(G, prog="twopi")
    nx.draw(G, pos)
    plt.show()
    return

def hierarchy_pos(G, root=None, width=1., vert_gap = 0.2, vert_loc = 0, leaf_vs_root_factor = 0.5):

    '''
    This function will return the positions to draw the graph of a network with hierarchical properties.

    This expects a directed graph with no loops. 

          
    We use use both of these approaches simultaneously with ``leaf_vs_root_factor`` 
    determining how much of the horizontal space is based on the bottom up 
    or top down approaches.  ``0`` gives pure bottom up, while 1 gives pure top
    down.   
    
    
    :Arguments: 
    
    **G** the graph (must be a tree)

    **root** the root node of the tree 
    - if the tree is directed and this is not given, the root will be found and used
    - if the tree is directed and this is given, then the positions will be 
      just for the descendants of this node.
    - if the tree is undirected and not given, then a random choice will be used.

    **width** horizontal space allocated for this branch - avoids overlap with other branches

    **vert_gap** gap between levels of hierarchy

    **vert_loc** vertical location of root
    
    **leaf_vs_root_factor**

    xcenter: horizontal location of root
    '''
  #  if not nx.is_tree(G):
  #      raise TypeError('cannot use hierarchy_pos on a graph that is not a tree')

    if root is None:
        if isinstance(G, nx.DiGraph):
            root = next(iter(nx.topological_sort(G)))  #allows back compatibility with nx version 1.11
        else:
            root = random.choice(list(G.nodes))

    def _hierarchy_pos(G, root, leftmost, width, leafdx = 0.2, vert_gap = 0.2, vert_loc = 0, 
                    xcenter = 0.5, rootpos = None, 
                    leafpos = None, parent = None):
        '''
        see hierarchy_pos docstring for most arguments

        pos: a dict saying where all nodes go if they have been assigned
        parent: parent of this branch. - only affects it if non-directed

        '''

        if rootpos is None:
            rootpos = {root:(xcenter,vert_loc)}
        else:
            rootpos[root] = (xcenter, vert_loc)
        if leafpos is None:
            leafpos = {}
        children = list(G.neighbors(root))
        leaf_count = 0
        if not isinstance(G, nx.DiGraph) and parent is not None:
            children.remove(parent)  
        if len(children)!=0:
            rootdx = width/len(children)
            nextx = xcenter - width/2 - rootdx/2
            for child in children:
                nextx += rootdx
                rootpos, leafpos, newleaves = _hierarchy_pos(G,child, leftmost+leaf_count*leafdx, 
                                    width=rootdx, leafdx=leafdx,
                                    vert_gap = vert_gap, vert_loc = vert_loc-vert_gap, 
                                    xcenter=nextx, rootpos=rootpos, leafpos=leafpos, parent = root)
                leaf_count += newleaves

            leftmostchild = min((x for x,y in [leafpos[child] for child in children]))
            rightmostchild = max((x for x,y in [leafpos[child] for child in children]))
            leafpos[root] = ((leftmostchild+rightmostchild)/2, vert_loc)
        else:
            leaf_count = 1
            leafpos[root]  = (leftmost, vert_loc)
#        pos[root] = (leftmost + (leaf_count-1)*dx/2., vert_loc)
#        print(leaf_count)
        return rootpos, leafpos, leaf_count

    xcenter = width/2.
    if isinstance(G, nx.DiGraph):
        leafcount = len([node for node in nx.descendants(G, root) if G.out_degree(node)==0])
    elif isinstance(G, nx.Graph):
        leafcount = len([node for node in nx.node_connected_component(G, root) if G.degree(node)==1 and node != root])
    rootpos, leafpos, leaf_count = _hierarchy_pos(G, root, 0, width, 
                                                    leafdx=width*1./leafcount, 
                                                    vert_gap=vert_gap, 
                                                    vert_loc = vert_loc, 
                                                    xcenter = xcenter)
    pos = {}
    for node in rootpos:
        pos[node] = (leaf_vs_root_factor*leafpos[node][0] + (1-leaf_vs_root_factor)*rootpos[node][0], leafpos[node][1]) 
#    pos = {node:(leaf_vs_root_factor*x1+(1-leaf_vs_root_factor)*x2, y1) for ((x1,y1), (x2,y2)) in (leafpos[node], rootpos[node]) for node in rootpos}
    xmax = max(x for x,y in pos.values())
    ymin = 0
    for node in pos:
        pos[node]= (pos[node][0]*width/xmax, pos[node][1])
        ymin = min(ymin,pos[node][1])
    #add unconnected nodes at the bottom
    unconnected_nodes = set(G.nodes)-set(rootpos.keys())
    
    
    for node in unconnected_nodes:
        pos[node] = (pos[root][0],ymin)
    return pos


def layered_hierarchy_pos(G, root=None, width=1., vert_gap = 0.2, vert_loc = 0, leaf_vs_root_factor = 0.5):

    '''
    This function will return the positions to draw the graph of a network with hierarchical properties.

    This expects a directed graph with no loops. 

          
    We use use both of these approaches simultaneously with ``leaf_vs_root_factor`` 
    determining how much of the horizontal space is based on the bottom up 
    or top down approaches.  ``0`` gives pure bottom up, while 1 gives pure top
    down.   
    
    
    :Arguments: 
    
    **G** the graph (must be a tree)

    **root** the root node of the tree 
    - if the tree is directed and this is not given, the root will be found and used
    - if the tree is directed and this is given, then the positions will be 
      just for the descendants of this node.
    - if the tree is undirected and not given, then a random choice will be used.

    **width** horizontal space allocated for this branch - avoids overlap with other branches

    **vert_gap** gap between levels of hierarchy

    **vert_loc** vertical location of root
    
    **leaf_vs_root_factor**

    xcenter: horizontal location of root
    '''
  #  if not nx.is_tree(G):
  #      raise TypeError('cannot use hierarchy_pos on a graph that is not a tree')

    if root is None:
        if isinstance(G, nx.DiGraph):
            root = next(iter(nx.topological_sort(G)))  #allows back compatibility with nx version 1.11
        else:
            root = random.choice(list(G.nodes))

    def _hierarchy_pos(G, root, leftmost, width, leafdx = 0.2, vert_gap = 0.2, vert_loc = 0, 
                    xcenter = 0.5, rootpos = None, 
                    leafpos = None, parent = None):
        '''
        see hierarchy_pos docstring for most arguments

        pos: a dict saying where all nodes go if they have been assigned
        parent: parent of this branch. - only affects it if non-directed

        '''

        if rootpos is None:
            rootpos = {root:(xcenter,vert_loc)}
        else:
            rootpos[root] = (xcenter, vert_loc)
        if leafpos is None:
            leafpos = {}
        children = list(G.neighbors(root))
        leaf_count = 0
        if not isinstance(G, nx.DiGraph) and parent is not None:
            children.remove(parent)  
        if len(children)!=0:
            rootdx = width/len(children)
            nextx = xcenter - width/2 - rootdx/2
            for child in children:
                nextx += rootdx
                rootpos, leafpos, newleaves = _hierarchy_pos(G,child, leftmost+leaf_count*leafdx, 
                                    width=rootdx, leafdx=leafdx,
                                    vert_gap = vert_gap, vert_loc = vert_loc-vert_gap, 
                                    xcenter=nextx, rootpos=rootpos, leafpos=leafpos, parent = root)
                leaf_count += newleaves

            leftmostchild = min((x for x,y in [leafpos[child] for child in children]))
            rightmostchild = max((x for x,y in [leafpos[child] for child in children]))
            leafpos[root] = ((leftmostchild+rightmostchild)/2, vert_loc)
        else:
            leaf_count = 1
            leafpos[root]  = (leftmost, vert_loc)
#        pos[root] = (leftmost + (leaf_count-1)*dx/2., vert_loc)
#        print(leaf_count)
        return rootpos, leafpos, leaf_count

    xcenter = width/2.
    if isinstance(G, nx.DiGraph):
        leafcount = len([node for node in nx.descendants(G, root) if G.out_degree(node)==0])
    elif isinstance(G, nx.Graph):
        leafcount = len([node for node in nx.node_connected_component(G, root) if G.degree(node)==1 and node != root])
    rootpos, leafpos, leaf_count = _hierarchy_pos(G, root, 0, width, 
                                                    leafdx=width*1./leafcount, 
                                                    vert_gap=vert_gap, 
                                                    vert_loc = vert_loc, 
                                                    xcenter = xcenter)
    pos = {}
    for node in rootpos:
        pos[node] = (leaf_vs_root_factor*leafpos[node][0] + (1-leaf_vs_root_factor)*rootpos[node][0], leafpos[node][1]) 
#    pos = {node:(leaf_vs_root_factor*x1+(1-leaf_vs_root_factor)*x2, y1) for ((x1,y1), (x2,y2)) in (leafpos[node], rootpos[node]) for node in rootpos}
    xmax = max(x for x,y in pos.values())
    ymin = 0
    for node in pos:
        pos[node]= (pos[node][0]*width/xmax, pos[node][1])
        ymin = min(ymin,pos[node][1])
    #add unconnected nodes at the bottom
    unconnected_nodes = set(G.nodes)-set(rootpos.keys())
    
    
    for node in unconnected_nodes:
        pos[node] = (pos[root][0],ymin)
    return pos


def main():
    plot_timeseries(0,91,datetime.datetime(2022,7,30,18,0,0))
    plot_timeseries(1,90,datetime.datetime(2022,8,2,10,0,0))
    plot_timeseries(2,83,datetime.datetime(2022,8,7,12,0,0))
    plot_timeseries(3,80,datetime.datetime(2022,8,14,12,0,0))

    plot_infecttree('infect_egdes_rev0.csv',63)

    #plot_infecttree('infect_egdes_gephi_rev0.csv')
    return

if __name__ == "__main__":
    main()