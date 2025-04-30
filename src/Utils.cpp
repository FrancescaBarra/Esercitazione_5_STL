#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
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
				mesh.MarkerCell0Ds[marker].push_back(id);;
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
			const auto it = mesh.MarkerCell1Ds.find(marker);
			if(it != mesh.MarkerCell1Ds.end())
			{
				mesh.MarkerCell1Ds[marker].push_back(id);
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
    }

    return true;
}

bool test_lati(PolygonalMesh& mesh)
{
	//Ciclo sulle celle 2D della mesh
	for(unsigned int i = 0; i < mesh.NumCell2Ds; i++)
	{
		//Ciclo sui lati della i-esima cella
		for(unsigned int j = 0; j < mesh.Cell2DsEdges[i].size(); j++)
		{	
			//Metto i lati della i-esima cella in un vettore
			vector<unsigned int>& lati = mesh.Cell2DsEdges[i];
			
            //Indici dei due estremi (iniziale e finale) del lato j-esimo
			int& indice_origine = mesh.Cell1DsExtrema(0,lati[j]);
			int& indice_fine = mesh.Cell1DsExtrema(1,lati[j]);
			
			//Coordinare dei punti estremi del lato j-esimo: x e y
			double& origine_x = mesh.Cell0DsCoordinates(0,indice_origine);
			double& origine_y = mesh.Cell0DsCoordinates(1,indice_origine);
			double& fine_x = mesh.Cell0DsCoordinates(0,indice_fine);
			double& fine_y = mesh.Cell0DsCoordinates(1,indice_fine);	
			
			double lunghezza = sqrt(pow(origine_x - fine_x,2) + pow(origine_y - fine_y, 2));
			
			if(lunghezza < 1e-16)
			{
				cout << "Errore: Il poligono con ID " << i << " ha il lato con ID " << lati[j] << " di lunghezza 0"<< endl;
				return false;
			}
		}
	}
	cout << "Nessun lato ha lunghezza 0" << endl;
	return true;
}


bool test_area(PolygonalMesh& mesh)
{
	//Ciclo sulle celle 2D della mesh
	for(unsigned int i = 0; i < mesh.NumCell2Ds; i++)
	{
		double area = 0.0;
		unsigned int n = mesh.Cell2DsVertices[i].size(); //--> numero di vertici del poligono i-esimo
		//Ciclo sui vertici del poligono i_esimo
		for(unsigned int j = 0; j < n; j++)
		{
			//Vertici adiacenti V1 e V2: cerco gli indici di V1 e V2
			unsigned int& V1_id = mesh.Cell2DsVertices[i][j];
			unsigned int& V2_id = mesh.Cell2DsVertices[i][(j+1)%n];
			
			//Coordinate dei vertici dei vertici V1 e V2
			double& V1_x = mesh.Cell0DsCoordinates(0,V1_id);
			double& V1_y = mesh.Cell0DsCoordinates(1,V1_id);
			double& V2_x = mesh.Cell0DsCoordinates(0,V2_id);
			double& V2_y = mesh.Cell0DsCoordinates(1,V2_id);
			
			area += V1_x * V2_y - V2_x * V1_y;
		}
		
		area = 0.5 * abs(area);
		
		if(area < 1e-12)
		{
			cout << "Errore: Il poligono con ID "<< i << " ha area pari a 0" << endl;
			return false;
		}
	}
	cout << "Non ci sono poligoni che hanno area pari a 0" << endl;
	return true;
}
}
