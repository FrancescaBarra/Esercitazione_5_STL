#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "Eigen/Eigen"

namespace PolygonalLibrary
{
bool ImportMesh(PolygonalMesh& mesh)
{

    if(!ImportCell0Ds(mesh))
        return false;

    if(!ImportCell1Ds(mesh))
        return false;

    if(!ImportCell2Ds(mesh))
        return false;

    return true;

}

bool ImportCell0Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell0Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;

    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    ///Tolgo la prima riga del file: Id;Marker;X;Y
    listLines.pop_front();

    mesh.NumCell0Ds = listLines.size();

    if (mesh.NumCell0Ds == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DsId.reserve(mesh.NumCell0Ds);
    mesh.Cell0DsCoordinates = Eigen::MatrixXd::Zero(3, mesh.NumCell0Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
        char sep;

        converter >> id >> sep >> marker >> sep >> mesh.Cell0DsCoordinates(0, id) >> sep >> mesh.Cell0DsCoordinates(1, id);

        mesh.Cell0DsId.push_back(id);

        /// Memorizza i marker
		if(marker != 0)
		{
			auto it = mesh.MarkerCell0Ds.find(marker);
			if(it != mesh.MarkerCell0Ds.end())
			{
				(*it).second.push_back(id);
			}
			else
			{
				mesh.MarkerCell0Ds.insert({marker, {id}});
			}
		}
		

    }

    return true;
}

bool ImportCell1Ds(PolygonalMesh& mesh)
{
    ifstream file("./Cell1Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
	
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    ///Tolgo la prima riga del file: Id;Marker;Origin;End
    listLines.pop_front();

    mesh.NumCell1Ds = listLines.size();

    if (mesh.NumCell1Ds == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DsId.reserve(mesh.NumCell1Ds);
    mesh.Cell1DsExtrema = Eigen::MatrixXi::Zero(2, mesh.NumCell1Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
        unsigned int marker;
		char sep;

        converter >> id >> sep >> marker >> sep >> mesh.Cell1DsExtrema(0, id) >> sep >> mesh.Cell1DsExtrema(1, id);
        mesh.Cell1DsId.push_back(id);

        /// Memorizza i marker
		if(marker != 0)
		{
			auto it = mesh.MarkerCell1Ds.find(marker);
			if(it != mesh.MarkerCell1Ds.end())
			{
				(*it).second.push_back(id);
			}
			else
			{
				mesh.MarkerCell1Ds.insert({marker, {id}});
			}
		}
    }

    return true;
}

bool ImportCell2Ds(PolygonalMesh& mesh)
{
    ifstream file;
    file.open("./Cell2Ds.csv");

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    file.close();

    ///Tolgo la prima riga del file: Id;Marker;NumVertices;Vertices;NumEdges;Edges
    listLines.pop_front();

    mesh.NumCell2Ds = listLines.size();

    if (mesh.NumCell2Ds == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DsId.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsVertices.reserve(mesh.NumCell2Ds);
    mesh.Cell2DsEdges.reserve(mesh.NumCell2Ds);

    for (const string& line : listLines)
    {
        istringstream converter(line);

        unsigned int id;
		unsigned int marker;
		unsigned int n_vertici;
		unsigned int n_lati;
		char sep;
		
		converter >> id >> sep >> marker >> sep >> n_vertici;
		
		vector<unsigned int> vector_vertici;
		vector_vertici.reserve(n_vertici);
		for(unsigned int i = 0; i < n_vertici; i++)
		{
			unsigned int vertice;
			converter >> sep >> vertice;
			vector_vertici.push_back(vertice);
		}
		mesh.Cell2DsVertices.push_back(vector_vertici);
		
		converter >> sep >> n_lati;
		vector<unsigned int> vector_lati;
		vector_lati.reserve(n_lati);
		for(unsigned int j = 0; j < n_lati; j++)
		{
			unsigned int lato;
			converter >> sep >> lato;
			vector_lati.push_back(lato);
		}
		mesh.Cell2DsEdges.push_back(vector_lati);
		
		mesh.Cell2DsId.push_back(id);
		
		/// Memorizza i marker
		if(marker != 0)
		{
			auto it = mesh.MarkerCell2Ds.find(marker);
			if(it != mesh.MarkerCell2Ds.end())
			{
				(*it).second.push_back(id);
			}
			else
			{
				mesh.MarkerCell2Ds.insert({marker, {id}});
			}
		}
    }

    return true;
}

}
