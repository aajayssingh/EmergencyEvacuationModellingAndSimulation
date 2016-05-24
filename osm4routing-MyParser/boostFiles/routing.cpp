/*
COMPILATION:
	cd Transportation\ Flow/boost_1_57_0/libs/graph/example
       g++ -I /home/kanchan/boost_1_57_0 routing.cpp -o routing
       ./routing

TODO:
1. Make graph bidirectional - DONE
2. connectivity matrix -DONE
3. gaussian distribution
4. source pruning
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <utility>
#include <iomanip>
#include <algorithm>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/config.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/connected_components.hpp>

#include <boost/ref.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
//#include <Eigen/Core>
#include <math.h>
#include <boost/math/distributions/normal.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

#define NUM_SOURCES 10
#define NUM_SINKS 10
#define INFINITY1 10
#define VERT 25




#ifndef WINDOWS
#include <sys/time.h>
#endif
#ifdef WINDOWS
  SYSTEMTIME      beg;
#else
  struct timeval  beg;
  struct tm      *t;
#endif


#include <stack>
#include<queue>
 using namespace std;

using boost::math::normal;
using namespace boost;
//using namespace Eigen;
//using namespace std;
using namespace boost::accumulators;


typedef std::pair<long int, long int> Edge;

int compare (const void * a, const void * b){
  		return ( *(long int*)a - *(long int*)b );
}

template <class Graph>
void print(Graph& g) {
  typename Graph::vertex_iterator i, end;
  typename Graph::out_edge_iterator ei, edge_end;
  for(boost::tie(i,end) = vertices(g); i != end; ++i) {
    cout << *i << " --> ";
    for (boost::tie(ei,edge_end) = out_edges(*i, g); ei != edge_end; ++ei)
      cout << target(*ei, g) << "  ";
    cout << endl;
  }
}



//original pdf  can be orig pdf or the orig pnodes
long int  find_index_of(vector<long double> pdf, long double key, int count_n)
{
 //cout <<"find index :"<<key<<endl;
 for(long int pi = 0 ; pi< count_n; pi++)
    {
      // cout <<"find index :"<<pdf[pi]<<" "<<key<<endl;
        if(pdf[pi] == key)
        {
            return pi;//index starts from 0
        }

    }
}




void writeto_osm(string ali)
{
    ifstream modifiedosmfile;
    string delimiter = "=", str_node,csvLine;
    size_t pos = 0;
    modifiedosmfile.open("map.osm",ifstream::in);
    ofstream osmrenderfile(ali);


    if (modifiedosmfile.is_open())
    {
        while( getline(modifiedosmfile, csvLine) )
        {    // read every line from the stream
         pos = csvLine.find(delimiter);

            {
                str_node = csvLine.substr(0, pos);
                    if(str_node.compare(" <way id"))
                    {
                        osmrenderfile << csvLine;
                        osmrenderfile<<endl;
                    }
                    else{
                        break;
                    }//str_node.str

            }


        }
        modifiedosmfile.close();
        osmrenderfile.close();
    }
    else
    {
    cout << "Error opening node.csv file"<<endl;
    }
}

int getNodecount()
{
    ifstream nodecountfile;
    nodecountfile.open("nodes.csv",ifstream::in);
    int total_nodes = 0;
    string csvLine;
    if (nodecountfile.is_open())
    {
        while( getline(nodecountfile, csvLine) )
        {    // read every line from the stream
            total_nodes++;
        }
    nodecountfile.close();
    }
    else
    {
        cout << "Error opening node.csv file"<<endl;
        return 1;
    }
//cout<<"total_nodes "<<total_nodes;
return(total_nodes -1);
}

int getEdgecount()
{
    ifstream nodecountfile;
    nodecountfile.open("edges.csv",ifstream::in);
    int total_nodes = 0;
    string csvLine;
    if (nodecountfile.is_open())
    {
        while( getline(nodecountfile, csvLine) )
        {    // read every line from the stream
            total_nodes++;
        }
    nodecountfile.close();
    }
    else
    {
        cout << "Error opening edges.csv file";
        return 1;
    }
//cout<<"total_nodes "<<total_nodes;
return(total_nodes -1);
}







long int  find_index_of_pnodes(vector<long int> pdf, long int key, int count_n)
{
 //cout <<"find index :"<<key<<endl;
 for(long int pi = 0 ; pi< count_n; pi++)
    {
      // cout <<"find index :"<<pdf[pi]<<" "<<key<<endl;
        if(pdf[pi] == key)
        {
            return pi;//index starts from 0
        }

    }
}
//sorted pdf , assume nth num comes beg from 1 not 0  1st largest = N - nth, N = count_nodes
int fetchNLargest(vector<long double> orig_pdf, vector<long double> sorted_pdf,int count_nth, int N)
{

return find_index_of(orig_pdf,sorted_pdf[N- count_nth],N);

}
//assume nth num comes beg from 1 not 0
int fetchNSmallest(vector<long double> orig_pdf, vector<long double> sorted_pdf,int count_nth, int N)
{

return find_index_of(orig_pdf,sorted_pdf[count_nth - 1],N);

}

//helper function to find if ele exist in array or not
int match(int arr[], int item)
{
		for(int i= 0 ;i<10;i++)
		{
		if(arr[i] == item)
		{
			//cout<<item<<"matched"<<arr[i]<<endl;
			return 1;
		}
		}
		return 0;

	}

	int matchsmall(int arr[], int item)
{
		for(int i= 0 ;i<25;i++)
		{
		if(arr[i] == item)
		{
			//cout<<item<<"matched"<<arr[i]<<endl;
			return 1;
		}
		}
		return 0;

	}


int lin_search(long int key, long int *pnode, int N)
{
    for(int i =0; i < N; i++)
    {
        if(key == pnode[i])
        {
            return i;
        }
    }

    return -1;
}


//fetches Ith largest index
int getIndexOfLargestElement(double arr[], int size, int k) {
    int largestIndex = 0;
    int skip_indexarr[10] ,indx = 0;

    for(int i =0 ;i<10;i++)
		skip_indexarr[i] = -1;

	//	for(int i =0 ;i<50;i++)
		//cout<<arr[i]<<endl;

    do{
    largestIndex = 0;
    for (int index = 0; index < size; index++) {

        if (arr[largestIndex] < arr[index]) {
        if(!match(skip_indexarr, index)){
            largestIndex = index;

        }
	}

    }
    skip_indexarr[indx++]=largestIndex;
    //cout<<"largest ele"<<largestIndex<<endl;

}while(indx != k);
    return largestIndex;
}
//fetches k th smallest element
int getIndexOfSmallestElement(double arr[], int size, int k) {
    int smallestIndex = 0;
   int skip_indexarr[25] ,indx = 0;

    for(int i =0 ;i<25;i++)
		skip_indexarr[i] = -1;

   do{
	   smallestIndex = 0;
    for (int index = 0; index < size; index++) {
        if (arr[smallestIndex] > arr[index]) {
            if(!matchsmall(skip_indexarr, index))
				smallestIndex = index;
        }
    }
    skip_indexarr[indx++]=smallestIndex;
    //cout<<"smallestIndex ele"<<smallestIndex<<endl;

}while(indx != k);
    return smallestIndex;
}


int searchAndUpdate(long int *node1, long int *node2, long int *pnodes,Edge *edge_array, int num_nodes,
   vector<long int> pnodes_orig_copy, vector <long double> pdf_xy_orig ,
    long double *Rij,long double *edge_capacity, long double *weights,long double lambda, long double *edge_capacity_norm , long double* distance_csv, long double* distance_csv_norm )
    {
        long int index1 = 0;
        long int index2 = 0;
        long double lambda2 = 0.3;

        long int n1,n2;
        int i = 0, j = 0 ;
        double max_edge_capacity;
        double min_edge_capacity;
        long double max_rij  ,min_rij;
        int num_edges=getEdgecount();
        int bidierational_edges = 2*num_edges;
        long int max_csvdistance, min_csvdistance;


        //search :
        for(i=0;i< bidierational_edges;i++)
        {
            index1 = lin_search(node1[i], pnodes, num_nodes);//pItem1 - pnodes;
            index2 = lin_search(node2[i], pnodes, num_nodes);
          //  cout<<"index1 and index 2  "<<node1[i]<<" "<<node2[i]<<endl;

            if ((-1 != index1) &&(-1 != index2))
            {
                edge_array[i] = std::make_pair (index1,index2);
                //cout<<"edge_array"<< edge_array[i].first<<" "<<edge_array[i].second<<endl;
            }
            else
                cout<<"edge is not in the array"<<endl;
        }

        //update
        for(i = 0; i<bidierational_edges; i++)
        {
            n1 = find_index_of_pnodes(pnodes_orig_copy, node1[i],num_nodes);//n1, n2 start from 0
            n2 = find_index_of_pnodes(pnodes_orig_copy, node2[i],num_nodes);
            //   cout<<"n1 ="<<n1<<" "<<"n2 = "<<n2<<endl;
            Rij[i] = pdf_xy_orig[n1] - pdf_xy_orig[n2];

            //rand() % 10+1; //
            //	cout<<pdf_xy_orig[n1]<<" "<<pdf_xy_orig[n2]<<" "<<Rij[i]<<endl;
        }

        max_rij = *max_element(Rij, Rij+bidierational_edges);
        min_rij = *min_element(Rij, Rij+bidierational_edges);
       // cout<<max_rij <<min_rij<<endl;

        for(i = 0; i<bidierational_edges; i++)
        {
            if(Rij[i] < 0)
            {
                Rij[i] = INFINITY1;//(-1)*(1/Rij[i]);
            }
            else
            {
                Rij[i] =(max_rij - min_rij )/(Rij[i] -  min_rij + 0.000001) ;// higer positive the value higer priority for that path , and for dijkstra need to make it small to choose such edge.
                if(i == 25)
                {
                  //  cout<<"Nr"<< (max_rij - min_rij )<<endl;
                   // cout<<"Dr"<< (Rij[i] -  min_rij + 0.000000001)<<endl;
                }
            }
          // cout<<"Rij["<<i<<"]: "<<Rij[i]<<endl;
        }

        for(i = 0; i<bidierational_edges; i+=2)
        {
            edge_capacity[i] = (rand()%100) +1;
            edge_capacity[i+1] = edge_capacity[i];
            // cout<<"edge_capacity["<<i<<"]: "<<edge_capacity[i]<<endl;
            // cout<<"edge_capacity["<<i+1<<"]: "<<edge_capacity[i+1]<<endl;
        }

        min_edge_capacity = *min_element(edge_capacity, edge_capacity+bidierational_edges);
        max_edge_capacity = *max_element(edge_capacity, edge_capacity+bidierational_edges);
       // cout<<"min_edge_capacity "<<min_edge_capacity<<endl;
       // cout<<"max_edgecapacity "<<max_edge_capacity <<endl;

        for(i = 0; i<bidierational_edges; i++)
        {
            edge_capacity_norm[i] = (edge_capacity[i] - min_edge_capacity + .000001)/(max_edge_capacity - min_edge_capacity + 0.000001);
           //cout<<"edge_capacity_norm["<<i<<"]: "<<edge_capacity_norm[i]<<endl;
        }

      ////  cout<<".............................................. ";

        max_csvdistance = *max_element(distance_csv, distance_csv+bidierational_edges);
        min_csvdistance = *min_element(distance_csv, distance_csv+bidierational_edges);
       // cout<<"max csv  min csv "<<max_csvdistance<<" "<<min_csvdistance<<endl;

        for(i = 0; i<bidierational_edges; i++)
        {
         //  cout<<"distance_csv["<<i<<"]: "<<distance_csv[i]<<endl;
           distance_csv_norm[i] = (distance_csv[i] - min_csvdistance +.000001 )/(max_csvdistance - min_csvdistance + 0.000001);
        // cout<<"distance_csv["<<i<<"]: "<<distance_csv[i]<<endl;
        }


//DECISION PARAMTER:  weighted average of disaster value and edge capacity.
        for(i=0;i<bidierational_edges;i++)
        {
          // cout<<"weights["<<i<<"]: "<<"Rij[i]  "<<"distance_csv[i] " <<weights[i]<<" "<<Rij[i]<<" "<<distance_csv[i]<<endl;
            weights[i] = (lambda*Rij[i]+
            (1-lambda-lambda2)*edge_capacity_norm[i] + lambda2*distance_csv_norm[i])*100;
            //weights[i+1] = lambda*Rij[i+1]+(1-lambda-lambda2)*edge_capacity_norm[i+1] + lambda2*distance_csv[i+1];

            //cout<<"weights["<<i<<"]: "<<weights[i]<<endl;
        }

    }


//namespace ublas = boost::numeric::ublas;
int main (int, char *[]) {
int node_num ;
//using namespace boost;//aj
//while(++paa !=10){
//cout<<"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"<<endl;

/**STEP1: EXTRACT EDGES FROM edges.csv ***/
    int i=0,j=0,n;
    char cNum[500]= {0} ;
    char cNum2[500] = {0};
    int  D[VERT][VERT] = {INFINITY1};
    long int check_path = 1;

    int way_id = 0;
    ofstream osmrenderfile;


    //char cnum3[500]

    long int node1[3500] = {0},node2[3500] = {0};
    long double distance_csv[3500] = {0};
    long double distance_csv_norm[3500] = {0};
    int count_edges=getEdgecount(),count_nodes= getNodecount();
    std::stack<int> mypath;
    std::stack<long double> mypathcopy;
    int node_index=0, edge_index = 0;
    ifstream nodecountfile;
    ifstream nodefile;
    string str_node;
    string::size_type sz;
    vector<vector< long double> > stat_arr(count_nodes, vector<long double>(2));
    istringstream line_stream;
    size_t pos = 0;
    string delimiter = ",";

    vector<long double> pdf_x(count_nodes);
    vector<long double> pdf_y(count_nodes);
    vector<long double> pdf_xy_copy(count_nodes);//sorted copy of pdf_xy
    vector<long double> pdf_xy_orig(count_nodes);//1 d copy of pdf_xy
    vector<long int> pnodes_orig_copy(count_nodes);//to match index of
    vector<int> source_list(NUM_SOURCES);
    vector<int> sink_list(NUM_SINKS);
    //vector<long double> pdf_xy(count_nodes);

    vector<vector< long double> > pdf_xy(count_nodes, vector<long double>(2));

    double rand_mean_x, rand_mean_y;
    long int rand_mean_indx;

    //using namespace boost;
    typedef adjacency_list < listS, vecS, directedS,no_property, property < edge_weight_t, int > > Graph;
    typedef graph_traits < Graph >::vertex_descriptor vertex_descriptor;
   // typedef graph_traits<Graph>::

    long double total_path_distance = 0;
    int bidierational_edges = 2*count_edges;

    Edge edge_array[bidierational_edges];
    //long int * pItem1 = NULL;
   // long int * pItem2 = NULL;
    long int index1 = 0;
    long int index2 = 0;
    int found_index = 0;
//    double max_edge_capacity;
//    double min_edge_capacity;

    long double Rij[bidierational_edges];
    //long int n1,n2;
    long double edge_capacity[bidierational_edges];
    long double edge_capacity_norm[bidierational_edges];

    long double weights[bidierational_edges];
    long double lambda = 0.5;
   long int node_capacity[count_nodes];
    int num_arcs = sizeof(edge_array) / sizeof(Edge);

    int num_sources =3;
    int source_node=0;
    int sink_node =0;
    int all_sinks=0;
    int no_path =0;
    int ajcountsmall =0;

    //int sink_index=0;
    long int num_comp;
    int count =0;
    long int effected_population =0;
    long int evacuated_population = 0;
    long double effective_time = 0;
    int nopath_flag =0;//@J
    long double path_lat, path_lon;
long int num_times_to_evacuate = 0;

    if((NUM_SINKS + NUM_SOURCES) >= count_nodes )
    {
        cout<<"YOU MESSED UP!  sources + sinks cannt be greater than total nodes for evacuation ";
        exit(0);
    }

    ifstream edgefile;
    string csvLine;
    writeto_osm("osmrender.osm");

    edgefile.open ("edges.csv", ifstream::in);

    	if (edgefile.is_open())
    	{
    		getline(edgefile, csvLine);
    		//while( getline(edgefile, csvLine)){    // read every line from the stream
			while (getline(edgefile, csvLine, '\n'))
			{   // read every element from the line that is seperated by commas

                while((pos = csvLine.find(delimiter)) != std::string::npos)
                {
                    str_node = csvLine.substr(0, pos);
                    //std::cout << str_node << " ";
                   // csvLine.erase(0, pos + delimiter.length());//   cout<<csvLine<<endl;

                    if(0 == i)
                    { i++;
                    csvLine.erase(0, pos + delimiter.length());}
                    else if(1==i)
                    {
                        node1[edge_index] = stod(str_node, &sz);
                        node2[edge_index+1] = stod(str_node, &sz);
                        csvLine.erase(0, pos + delimiter.length());
                        i++;
                    }
                    else if(2 == i)
                    {
                        node2[edge_index] = stod(str_node, &sz);
                        node1[edge_index+1] = stod(str_node, &sz); // cout<<j<<" "<<stod(str_node, &sz)<<endl;
                        i++;
                        csvLine.erase(0, pos + delimiter.length());
                    }
                    else if(3 == i)
                    {
                    distance_csv[edge_index]  = stod(str_node, &sz);
                    distance_csv[edge_index + 1]  = stod(str_node, &sz);                // cout<<j<<" "<<stod(str_node, &sz)<<endl;
                   // cout<<"distance_csv["<<edge_index<<"]: "<<distance_csv[edge_index]<<endl;
                    csvLine.erase(0, pos + delimiter.length());
                    //cout<<"ashdyfsdcfyucbgfdgggggggggffffffffffffffffffffffffffffffffffffffffffffff";
                    i++;
                    }
                    else
                    {
                        i=0;
                        csvLine.clear();
                        edge_index+=2;
                        break;
                    }

			}

   	}

   edgefile.close();
   	}
   	else{
   	    cout << "Error opening file";
   	    return 1;
   	}

	cout<<"Number of Roads in Map: "<<count_edges/2<<endl;


/*CALCULATE NUMBER OF NODES FROM node.csv FILE **/


	cout<<"Intersection Points: "<<count_nodes<<endl;

/*EXTRACT NODES FROM nodes.csv file AND SAVE */     					///// need to free this memory.

    	nodefile.open("nodes.csv",ifstream::in);
    	long int * pnodes;
    	// long int **stat_arr;// to store the (lat , lon ) vectors in array
    	//long int pnodes[count_nodes];
    	pnodes= new (nothrow) long int[count_nodes];
    	//string::size_type sz;
    	//char *sz;
    	//long int **stat_arr = new (nothrow) long int *[count_nodes];

    	//vector<vector<long double> > pdf_xy(count)
//cout <<"321count_nodes="<<count_nodes<<endl;
    	if (pnodes == NULL)
    	{
    		cout << "Error: memory could not be allocated";
    		}
    	else{

if (nodefile.is_open())
{
    getline(nodefile, csvLine);//to skip header line

    while(getline(nodefile, csvLine) )
    {    // read every line from the stream

        //cout<<csvLine<<endl;
        while((pos = csvLine.find(delimiter)) != std::string::npos)
        {
            str_node = csvLine.substr(0, pos);
            //std::cout << str_node << " ";
            csvLine.erase(0, pos + delimiter.length());//   cout<<csvLine<<endl;

            if(0==j)
            {
                pnodes[node_index] = stod(str_node, &sz);
                //  cout<<j<<" "<<stod(str_node, &sz)<<endl;
            }
            else if(1 == j)
            {
               // cout<<"nodee_index"<<node_index<<endl;
                stat_arr[node_index][0] = stod(str_node, &sz);
                // cout<<j<<" "<<stod(str_node, &sz)<<endl;
            }
            j++;
        }
        if(2== j)
        {
            stat_arr[node_index][1] = stod(csvLine, &sz);
            //cout<<j<<" "<<stod(csvLine, &sz)<<endl;
        }
        j=0;
        node_index++;
    }
    nodefile.close();
}
    else{
        cout << "Error opening node.csv file";
        return 1;
    }
	}

//cout<<"nodeindex"<<node_index<<endl;

	//begin void stats_cal()
	{
	 accumulator_set<
        double, features<tag::variance > > acc_x;

        accumulator_set<
        double, features<tag::variance > > acc_y;

	 double std_dvx, std_dvy, mean_x, mean_y, std_dvxy;

	for(int i =0; i< count_nodes; i++)
    {
    // cout<<stat_arr[i][0]<<" "<<stat_arr[i][1] <<" "<<endl;
        acc_x(stat_arr[i][0]);
        acc_y(stat_arr[i][1]);
    }
        //mean_x = mean(acc_x);
//        std_dvx = sqrt(accumulators::moment<2>(acc_x))/(13/*count_nodes*/ - 1);
        std_dvx = sqrt(variance(acc_x));
       // mean_y= mean(acc_y);
        std_dvy = sqrt(variance(acc_y));

    std_dvxy = sqrt((variance(acc_x) + variance(acc_y))/2);


        //cout<<"mean = "<<mean(acc_x)<<" "<<mean(acc_y)<<endl;
        cout<<endl<<"variance x  y  std_dvxy= "<<std_dvx<<" "<<std_dvy<<" "<<std_dvxy<< std::endl;


        //generating random impact point as mean
        srand (time(NULL));
        rand_mean_indx = rand() % count_nodes+1;
        rand_mean_y = stat_arr[rand_mean_indx][1];
        rand_mean_x = stat_arr[rand_mean_indx][0];

       cout<<"rand meanx and y n indx "<<rand_mean_x<<" "<<rand_mean_y<<" "<<rand_mean_indx<< std::endl;

//Generate the normal distribution
normal packs_x(rand_mean_x, std_dvy);

normal packs_y(rand_mean_y, std_dvy);


//generating pdf for each sample point

// cout<<"pdf for x"<<endl;
for(int pi = 0 ; pi< count_nodes; pi++)
{

       // cout<<" "<<stat_arr[pi][0]<<" ";
       pdf_x[pi] = pdf(packs_x,stat_arr[pi][0]);
       // std::cout << pdf(packs_x,stat_arr[pi][0]) << std::endl;
}


 //cout<<"pdf for y"<<endl;
for(int pi = 0 ; pi< count_nodes; pi++)
{
        pdf_y[pi] = pdf(packs_y,stat_arr[pi][1]);
    //  std::cout << pdf(packs_y,stat_arr[pi][1]) << std::endl;
}

//root mean square
cout <<"441count_nodes="<<count_nodes<<endl;
//cout<<endl<<"pdf for xy"<<endl;
for(int pi = 0 ; pi< count_nodes; pi++)
{
    pdf_xy[pi][0] = sqrt(((pdf_x[pi]*pdf_x[pi]) + (pdf_y[pi]*pdf_y[pi]))/2);
    pdf_xy[pi][1] = pi;
    pdf_xy_copy[pi] = pdf_xy[pi][0];
    pdf_xy_orig[pi] = pdf_xy[pi][0];
    pnodes_orig_copy[pi] = pnodes[pi];
   // cout<<pdf_xy[pi][0]<<endl;
}

sort(pdf_xy_copy.begin(), pdf_xy_copy.end());

//cout<<"print orig pnodes";
//for(int pi = 0 ; pi< count_nodes; pi++)
//{
////cout<<pdf_xy[pi][0]<<" "<<pdf_xy[pi][1]<<endl;
//cout<<pnodes_orig_copy[pi]<<" "<<pnodes[pi]<<endl;
////cout<<pdf_xy_orig[pi]<<" "<<pdf_xy_copy[pi]<<endl;
//}


	}// end stats_cal

//linear search
//long int orig_index = find_index_of(pdf_xy_orig,pdf_xy_copy[0], count_nodes);
//cout<<"orig_index"<<orig_index;


/*SORT NODES */
 	//sort(pnodes, pnodes+count_nodes);
 	//print sorted nodes
// 	for(int m=0;m<count_nodes;m++){
// 		cout<<" sorted node: "<<pnodes[m]<<endl;
// 	}
//#if 0



node_num = getNodecount();

//RADIATION LEVEL - vm
	/*Gausian distribution*/
	cout<<endl;


srand (time(NULL));
cout << "bidierational_edges "<<bidierational_edges<<endl;
for(int i = 0; i< bidierational_edges; i++ )
{
    weights[i] = INFINITY1;
}

int k =searchAndUpdate(node1, node2, pnodes, edge_array, count_nodes,
pnodes_orig_copy, pdf_xy_orig, Rij, edge_capacity, weights, lambda, edge_capacity_norm, distance_csv, distance_csv_norm);



cout <<"588count_nodes="<<node_num<<endl;
	//cout<<"pdf_xy_orig[n1]<<" "<<pdf_xy_orig[n2]<<" "<<Rij[i]"<<endl;





	for(i = 0; i<node_num; i++){
	  	node_capacity[i] = rand() % 1000+1;// population  at the node

	}

	cout <<"node_num="<<node_num<<endl;


 	Graph g(edge_array, edge_array + num_arcs, weights, node_num),
 	gcopy(edge_array, edge_array + num_arcs, weights, node_num);

  	int name[bidierational_edges];
  	for(i=0; i< bidierational_edges; i++){
  		name[i] = int(i);
  	}


 std::vector<std::size_t> parent(count_nodes-1);

	 for (i = 0; i < count_nodes; ++i)
             parent[i] = i;
       //  distance[0] = 0;

	property_map<Graph, edge_weight_t>::type weightmap = get(edge_weight, g);

//	  for (std::property_map<Graph, edge_weight_t>::iterator i = weightmap.begin();
//       i != weightmap.end(); ++i)
//    std::cout << i->first << ": " << i->second << "\n";



    std::vector<vertex_descriptor> p(num_vertices(g));
    std::vector<long int> d(num_vertices(g));

	//Select Sourse

	int paths[bidierational_edges];
	for(i=0; i< bidierational_edges; i++){
  		paths[i] = i;
  	}

  	for(int i = 0; i < NUM_SOURCES; i++)
    {
        source_list[i] = fetchNLargest(pdf_xy_orig, pdf_xy_copy, i+1 ,node_num);
        cout<<"sources "<<source_list[i]<<endl;
    }

    for(int i = 0; i < NUM_SINKS; i++)
    {
        sink_list[i] = fetchNSmallest(pdf_xy_orig, pdf_xy_copy, i+1 ,node_num);
         cout<<"sinks "<<sink_list[i]<<endl;
    }

    johnson_all_pairs_shortest_paths(g, D, distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

        std::cout << "       ";
        for (int k = 0; k < VERT; ++k)
            std::cout << std::setw(5) << k;
        std::cout << std::endl;

        for (int i = 0; i < VERT; ++i)
        {
            std::cout << std::setw(3) << i << " -> ";
            for (int j = 0; j < VERT; ++j)
            {
                if (D[i][j] == (std::numeric_limits<int>::max)())
                    std::cout << std::setw(5) << "inf";
                else
                    std::cout << std::setw(5) << D[i][j];
            }
            std::cout << std::endl;
        }


for(int ajcount = 0; ajcount < NUM_SOURCES; ajcount++)
{
    effected_population += node_capacity[source_list[ajcount]];
}


osmrenderfile.open("osmrender.osm", ios::app);
int ajcountsink = 0;
long int optimal_sink_index ;
long int sink_index;
long int optimal_sink_value = INFINITY;
  	for(int ajcount = 0; ajcount < NUM_SOURCES; ajcount++)
  	{//@J
        cout<<endl<<endl<<endl;
optimal_sink_index = -1;
 sink_index = -1;
optimal_sink_value = INFINITY;


        source_node = source_list[ajcount];//fetchNLargest(pdf_xy_orig, pdf_xy_copy, ajcount+1 ,node_num);//getIndexOfLargestElement(weights, node_num, ajcount+1);
       // sink_node = sink_list[ajcount];





for(sink_index = 0; sink_index < NUM_SINKS; sink_index++)
{

if(source_node == sink_list[sink_index] || (INFINITY == D[source_node][sink_list[sink_index]] ))
{
    continue;
}

    if(optimal_sink_value > D[source_node][sink_list[sink_index]])
    {
        optimal_sink_value = D[source_node][sink_list[sink_index]];
        optimal_sink_index  = sink_index;
    }
}

 if(-1 == optimal_sink_index)
 {
    cout<<"source"<<source_node<< "is not connected to any of the chosen sinks  "<<endl ;
    continue;//go for next source
 }
 //{
    sink_node = sink_list[optimal_sink_index];
      no_path = 0;
	{// to fix segmentation fault remove do while loop and connected comp code
		std::vector<long int> component(num_vertices(g));
		num_comp = connected_components(g, &component[0]);
		std::vector<int>::size_type i_comp;
		//cout << "Total number of components: " << num_comp << endl;

        if(component[source_node]!= component[sink_node])
        {
            no_path = 1;
            cout<<source_node<<" and "<<sink_node<<"are not connected"<<endl;
            continue;
            //ajcountsmall++;
        }
        else
        {
            cout<<source_node<<" and "<<sink_node<<"are connected"<<endl;
            no_path = 0;
            //break;
        }
    }


//        while(source_node != sink_node )
//        {
           //count++;
           vertex_descriptor s = vertex(source_node, gcopy);

            dijkstra_shortest_paths(gcopy, s,
            predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, gcopy))).
            distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, gcopy))));

            //std::cout << "distances and parents:" << std::endl;
           // graph_traits < Graph >::vertex_iterator vi, vend;
            //j = sink_node;
//#if 0
//#endif
#if 0
std::cout << "distances and parents:" << std::endl;
    graph_traits < Graph >::vertex_iterator vi, vend;

    for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi)
    {
        std::cout << "distance(" << *vi << ") = " << d[*vi] << ", ";
        std::cout << "parent(" << *vi << ") = " << p[*vi]<< std::
        endl;
    }
#endif

    i= sink_node;
    long double min_capacity = INFINITY;
    mypath.push(sink_node);//sink node is not pushed in below code
    while((i != source_node)&&( check_path <= count_nodes)&&(!no_path))
    {

        for(found_index = 0; found_index < bidierational_edges; found_index++)
        {
            if(edge_array[found_index].first == p[i] && edge_array[found_index].second == i)
            {

                if(edge_capacity[found_index] < min_capacity)
                {
                    min_capacity = edge_capacity[found_index];
                }
                mypathcopy.push(distance_csv[found_index]);
            }

        }
        cout<<"pushing "<<p[i]<<endl;
        mypath.push(p[i]);
        i = p[i];
        check_path++;

    }

    if(min_capacity == 0)
    {
        cout<<"MIN Capacity 0---> divide by zero";
    }

num_times_to_evacuate = node_capacity[source_node]/min_capacity;




// long int edge_source = -1  ;
//long int edge_destination = -1;
cout<<"min capacity in the path "<<min_capacity<<endl;
long int prev_top = -1;
//long double prev_top_copy = -1;

    cout<<"printing path and disance "<<endl;
    total_path_distance = 0;
	long int node_id ;


    path_lat = stat_arr[mypath.top()][0];
    path_lon = stat_arr[mypath.top()][1];

osmrenderfile<<" <way id="<<"\""<<way_id<<"\""<<" visible=\"true\">"<<endl;

    node_id = pnodes[mypath.top()] ;
    osmrenderfile<<"  <nd ref="<<"\""<<node_id<<"\""<<"/>"<<endl;

while((!mypath.empty())&&!(mypathcopy.empty()))
{
    //int k = mypath.pop();
    prev_top = mypath.top();

    mypath.pop();

    if((mypath.empty()))
        break;

    total_path_distance += mypathcopy.top();
    cout<<prev_top<<" ---> "<<mypath.top()<<" "<<path_lat<<" "<<path_lon<<"    "<<mypathcopy.top()<<endl;
    mypathcopy.pop();
    path_lat = stat_arr[mypath.top()][0];
    path_lon = stat_arr[mypath.top()][1];
    node_id = pnodes[mypath.top()] ;
    osmrenderfile<<"  <nd ref="<<"\""<<node_id<<"\""<<"/>"<<endl;


}


osmrenderfile<<" <tag k=\"highway\" v=\"AJAY_DEB\"/>"<<endl;
osmrenderfile<<" <tag k=\"name\" v=\"AJAY_DEB-name\"/>"<<endl;
 osmrenderfile<<" </way>"<<endl;
 way_id++;
//	#ifdef WINDOWS
//		GetLocalTime( &beg );
//
//		printf( "At time : %02d:%02d:%02d:%06d\n", beg.wHour,
//		beg.wMinute, beg.wSecond, beg.wMilliseconds * 1000 );
//	#else
//		gettimeofday( &beg, NULL );
//		t = localtime( &beg.tv_sec );
//
//		printf( "At time : %02d:%02d:%02d.%06d\n",
//		t->tm_hour, t->tm_min, t->tm_sec, (int) beg.tv_usec );
//	#endif

cout<<"total_path_distance for the path  "<<total_path_distance<<endl;

evacuated_population += node_capacity[source_node];
if(effective_time < num_times_to_evacuate*total_path_distance )
{
    effective_time = num_times_to_evacuate*total_path_distance;
}

	cout<<endl<<node_capacity[source_node]<<" people evacuated in this path "<<"in total time "
	<<num_times_to_evacuate*total_path_distance <<"seconds"<<endl;
}//@J


osmrenderfile<<"</osm>";
osmrenderfile.close();

cout<<"Population Effected  "<<effected_population<<endl;
cout<<"Population Evacuated   "<<evacuated_population<<"in net time "<<effective_time<<" seconds"<<endl;


//Graph Visualization
	std::ofstream dot_file("figs/routing1.dot");

	dot_file << "digraph D {\n"
	    << "  rankdir=LR\n"
	    << "  size=\"4,3\"\n"
	    << "  ratio=\"fill\"\n"
	    << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";


	graph_traits < Graph >::edge_iterator ei, ei_end;
	for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
		graph_traits < Graph >::edge_descriptor e = *ei;
	    	graph_traits < Graph >::vertex_descriptor u = source(e, g), v = target(e, g);
	    //	graph_traits < Graph >::vertex_descriptor u = source_node, v = sink_node;
	    	dot_file << name[u] << " -> " << name[v]<< "[label=\"" << get(weightmap, e) << "\"";
	    	if (p[v] == u)
	      		dot_file << ", color=\"black\"";
	    	else
	      		dot_file << ", color=\"grey\"";
	    	dot_file << "]";
  	}
  	dot_file << "}";

  //	delete[] pnodes;
//#endif
//}
  	return EXIT_SUCCESS;

 }
