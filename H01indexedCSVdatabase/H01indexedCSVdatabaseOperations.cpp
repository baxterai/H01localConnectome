/*******************************************************************************
 *
 * File Name: H01indexedCSVdatabaseOperations.cpp
 * Author: Richard Bruce Baxter - Copyright (c) 2021-2023 Baxter AI (baxterai.com)
 * License: MIT License
 * Project: H01LocalConnectome
 * Requirements: see H01indexedCSVdatabaseGlobalDefs.hpp
 * Compilation: see H01indexedCSVdatabaseGlobalDefs.hpp
 * Usage: see H01indexedCSVdatabaseGlobalDefs.hpp
 * Description: H01 indexed CSV database operations
 * Comments:
 * /
 *******************************************************************************/

#include "H01indexedCSVdatabaseOperations.hpp"

double H01indexedCSVdatabaseOperationsClass::calibrateCoordinateX(const double csvDatabaseCoordinateX)
{
	double visualisationCoordinateX = (csvDatabaseCoordinateX*LOCAL_CONNECTOME_VISUALISATION_CALIBRATION_FACTOR_X) + LOCAL_CONNECTOME_VISUALISATION_CALIBRATION_MIN_X;
	return visualisationCoordinateX;
}
double H01indexedCSVdatabaseOperationsClass::calibrateCoordinateY(const double csvDatabaseCoordinateY)
{
	double visualisationCoordinateY = (csvDatabaseCoordinateY*LOCAL_CONNECTOME_VISUALISATION_CALIBRATION_FACTOR_Y) + LOCAL_CONNECTOME_VISUALISATION_CALIBRATION_MIN_Y;
	return visualisationCoordinateY;
}
double H01indexedCSVdatabaseOperationsClass::calibrateCoordinateZ(const double csvDatabaseCoordinateZ)
{
	double visualisationCoordinateZ = (csvDatabaseCoordinateZ*LOCAL_CONNECTOME_VISUALISATION_CALIBRATION_FACTOR_Z) + LOCAL_CONNECTOME_VISUALISATION_CALIBRATION_MIN_Z;
	return visualisationCoordinateZ;
}

bool H01indexedCSVdatabaseOperationsClass::readLocalConnectomeNeuronsCSVdataset(const string LocalConnectomeCSVdatasetFileName, vector<vector<string>>* localConnectomeNeuronsCSVdataset, int* localConnectomeNeuronsCSVdatasetSize, vector<string>* neuronList, map<string, int>* neuronMap)
{
	SHAREDvars.getLinesFromFileCSV(LocalConnectomeCSVdatasetFileName, localConnectomeNeuronsCSVdataset, localConnectomeNeuronsCSVdatasetSize, CSV_DELIMITER_CHAR, true);
	
	#ifdef LOCAL_CONNECTOME_OFFICAL_RELEASE_C3_SOMAS_EXPECT_STRING_DELIMITERS
	for(int i=0; i<localConnectomeNeuronsCSVdataset->size(); i++)
	{
		vector<string>* localConnectomeNeuron = &((*localConnectomeNeuronsCSVdataset)[i]);
		//reformat string cells;
		for(int j=0; j<localConnectomeNeuron->size(); j++)
		{
			string cell = (*localConnectomeNeuron)[j];	
			string cellFormatted = "";
			if((cell[0] == STRING_DELIMITER) && (cell[cell.length()-1] == STRING_DELIMITER))
			{
				cellFormatted = cell.substr(1, cell.length()-2);
				//cout << "cellFormatted = " << cellFormatted << endl;
			}
			(*localConnectomeNeuron)[j] = cellFormatted;
		}
	}
	#endif
	#ifdef LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_LAYERS
	for(int i=0; i<localConnectomeNeuronsCSVdataset->size(); i++)
	{
		vector<string>* localConnectomeNeuron = &((*localConnectomeNeuronsCSVdataset)[i]);
		//reformat layer cells;
		string layerString = (*localConnectomeNeuron)[LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_ARTIFICIAL_LAYER];
		#ifdef LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_LAYERS_STRING
		int layerIndex = INT_DEFAULT_VALUE;	
		//cout << "layerString = " << layerString << endl;
		if(SHAREDvars.textInTextArray(layerString, somasLayerNames, CORTICAL_LAYER_NUMBER_OF_LAYERS, &layerIndex))
		{
			layerIndex++;
		}
		else
		{
			//"unclassified", "NULL"
			layerIndex = 0;
			//cerr << "LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_LAYERS: H01indexedCSVdatabaseOperationsClass::readLocalConnectomeNeuronsCSVdataset error: !textInTextArray, layerIndex name unknown; layerString = " << layerString << endl;
		}
		//cout << "layerIndex = " << layerIndex << endl;
		#else
		int layerIndex = SHAREDvars.convertStringToInt(layerString);
		#endif
		(*localConnectomeNeuron)[LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_ARTIFICIAL_LAYER] = SHAREDvars.convertIntToString(layerIndex);
	}
	#endif
	#ifdef LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_EXCITATION_TYPE_ARTIFICIAL
	#ifndef INDEXED_CSV_DATABASE_LDC	//excitationType calculations by LDC use neuronTypes.txt
	for(int i=0; i<localConnectomeNeuronsCSVdataset->size(); i++)
	{
		vector<string>* localConnectomeNeuron = &((*localConnectomeNeuronsCSVdataset)[i]);	
		string neuronType = (*localConnectomeNeuron)[LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_TYPE];
		string neuronExcitationTypeString = SHAREDvars.convertIntToString(LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_EXCITATION_TYPE_UNKNOWN);
		if(neuronType == LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_TYPE_PYRAMIDAL)
		{
			neuronExcitationTypeString = SHAREDvars.convertIntToString(LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_EXCITATION_TYPE_EXCITATORY);
		}
		else if(neuronType == LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_TYPE_INTERNEURON)
		{
			neuronExcitationTypeString = SHAREDvars.convertIntToString(LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_EXCITATION_TYPE_INHIBITORY);
		}
		else
		{
			neuronExcitationTypeString = SHAREDvars.convertIntToString(LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_EXCITATION_TYPE_UNKNOWN);		
		}
		//cout << "neuronType = " << neuronType << endl;
		//cout << "neuronExcitationTypeString = " << neuronExcitationTypeString << endl;
		localConnectomeNeuron->push_back(neuronExcitationTypeString);	//LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_EXCITATION_TYPE	
	}
	#endif
	#endif
	#ifdef LOCAL_CONNECTOME_DATASET_NEURONS_IGNORE_ADDITIONAL_NEURON_TYPES
	for(int i=0; i<localConnectomeNeuronsCSVdataset->size(); i++)
	{	
		vector<string>* localConnectomeNeuron = &((*localConnectomeNeuronsCSVdataset)[i]);
		int neuronExcitationType = SHAREDvars.convertStringToInt((*localConnectomeNeuron)[LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_EXCITATION_TYPE]);
		if(neuronExcitationType == LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_EXCITATION_TYPE_UNKNOWN)
		{
			//remove neuron from localConnectomeNeuronsCSVdataset
			//cout << "remove neuron from localConnectomeNeuronsCSVdataset" << endl;
			localConnectomeNeuronsCSVdataset->erase(localConnectomeNeuronsCSVdataset->begin() + i);
			i--;
		}
	}
	#endif

	for(int i=0; i<localConnectomeNeuronsCSVdataset->size(); i++)
	{
		vector<string>* localConnectomeNeuron = &((*localConnectomeNeuronsCSVdataset)[i]);
		
		//populate neuronList;
		string neuronID = (*localConnectomeNeuron)[LOCAL_CONNECTOME_DATASET_NEURONS_FIELD_INDEX_NEURON_ID];
		//cout << "neuronID = " << neuronID << endl;
		neuronList->push_back(neuronID);		
	}
	
	cout << "neuronList->size() = " << neuronList->size() << endl;
		
	#ifdef INDEXED_CSV_DATABASE_QUERY_EFFICIENT_STORE_NEURON_IDS_IN_MAP
	//populate neuronMap;
	for(int i=0; i < neuronList->size(); i++)
	{
    	(*neuronMap)[(*neuronList)[i]] = i;
	}
	#endif
}

bool H01indexedCSVdatabaseOperationsClass::readLocalConnectomeConnectionsCSVdataset(const string LocalConnectomeCSVdatasetFileName, vector<vector<string>>* localConnectomeConnectionsCSVdataset, int* localConnectomeConnectionsCSVdatasetSize)
{
	return readLocalConnectomeConnectionsCSVdataset(LocalConnectomeCSVdatasetFileName, localConnectomeConnectionsCSVdataset, localConnectomeConnectionsCSVdatasetSize, false, NULL);
}
bool H01indexedCSVdatabaseOperationsClass::readLocalConnectomeConnectionsCSVdataset(const string LocalConnectomeCSVdatasetFileName, vector<vector<string>>* localConnectomeConnectionsCSVdataset, int* localConnectomeConnectionsCSVdatasetSize, bool buildConnectionsMap, map<string, int>* connectionsMap)
{
	SHAREDvars.getLinesFromFileCSV(LocalConnectomeCSVdatasetFileName, localConnectomeConnectionsCSVdataset, localConnectomeConnectionsCSVdatasetSize, CSV_DELIMITER_CHAR, true);

	if(buildConnectionsMap)
	{
		for(int i=0; i < localConnectomeConnectionsCSVdataset->size(); i++)
		{
			vector<string>* localConnectomeConnection = &((*localConnectomeConnectionsCSVdataset)[i]);
			string connectionsMapKey = (*localConnectomeConnection)[LOCAL_CONNECTOME_DATASET_CONNECTIONS_FIELD_INDEX_PRE_ID] + (*localConnectomeConnection)[LOCAL_CONNECTOME_DATASET_CONNECTIONS_FIELD_INDEX_POST_ID];
			//cout << "connectionsMapKey to insert = " << connectionsMapKey << endl;
    		(*connectionsMap)[connectionsMapKey] = i;
		}
	}	
}

void H01indexedCSVdatabaseOperationsClass::prepareLocalConnectomeDatasetRead(const string local_connectome_folder_base)
{
	#ifdef INDEXED_CSV_DATABASE_QUERY_READ_CURRENT_FOLDER
	const string indexedCSVdatabaseQueryInputFolder = currentDirectory;	
	SHAREDvars.setCurrentDirectory(indexedCSVdatabaseQueryInputFolder);
	cout << "indexedCSVdatabaseQueryInputFolder = " << indexedCSVdatabaseQueryInputFolder << endl;
	#else
	prepareLocalConnectomeDatasetReadCustom(local_connectome_folder_base, LOCAL_CONNECTOME_DATASET_FOLDER);
	#endif
}

void H01indexedCSVdatabaseOperationsClass::prepareLocalConnectomeDatasetReadCustom(const string local_connectome_folder_base, const string folderName)
{
	#ifdef LOCAL_CONNECTOME_FOLDER_BASE_USE_RELATIVE_FOLDER
	SHAREDvars.setCurrentDirectory(currentDirectory);
	#endif
	string localConnectomeCSVdatasetFolder = local_connectome_folder_base;
	SHAREDvars.setCurrentDirectory(localConnectomeCSVdatasetFolder);
	localConnectomeCSVdatasetFolder = folderName;
	SHAREDvars.setCurrentDirectory(localConnectomeCSVdatasetFolder);
}


ofstream H01indexedCSVdatabaseOperationsClass::prepareLocalConnectomeDatasetWrite(const bool write, const bool appendToFile, const string datasetFileNameWrite)
{
	#ifdef INDEXED_CSV_DATABASE_QUERY_WRITE_CURRENT_FOLDER
	const string indexedCSVdatabaseQueryOutputFolder = currentDirectory;	
	#else
	const string indexedCSVdatabaseQueryOutputFolder = INDEXED_CSV_DATABASE_QUERY_OUTPUT_FOLDER;
	#endif
	
	ofstream writeFileObject;
	
	ios_base::openmode writeMode;
	if(appendToFile)
	{
		writeMode = ofstream::app;
	}
	else
	{
		writeMode = ofstream::out; 
	}
	if(write)
	{
		SHAREDvars.setCurrentDirectory(indexedCSVdatabaseQueryOutputFolder);
		writeFileObject.open(datasetFileNameWrite.c_str(), writeMode);
	}
	
	return writeFileObject;
}

void H01indexedCSVdatabaseOperationsClass::finaliseLocalConnectomeDatasetWrite(const bool write, ofstream* writeFileObject)
{
	#ifdef INDEXED_CSV_DATABASE_QUERY_OUTPUT
	writeFileObject->close();
	#endif
}
	
