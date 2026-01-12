#include<iostream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <string.h>

#include <fstream>
#include <sstream>

using namespace std;

struct Process{
    string name;
    unsigned int arrivalTime;
    unsigned int memoryRequirement;
    unsigned int executionTimeUnits;
    unsigned int remainingTimeUnits;
};

struct CPU{
    vector <Process> queue;
    vector <Process> runningProcesses;
    unsigned int totalMemory;
};

void loadProcessesFromFile(string importFileName, vector<Process> &processes){
    ifstream inputFile;
    
    inputFile.open(importFileName.c_str(), ios::in);
    
    if(inputFile.is_open()){
        string currentFileLine="";
        
        //Leemos currentFileLine a currentFileLine
        while(getline(inputFile, currentFileLine)){
            stringstream ss(currentFileLine);
            Process loadedProcess;
            
            ss >> loadedProcess.name >> loadedProcess.arrivalTime >> loadedProcess.memoryRequirement >> loadedProcess.executionTimeUnits;
            loadedProcess.remainingTimeUnits = loadedProcess.executionTimeUnits;
            
            processes.push_back(loadedProcess);
        }
        
        inputFile.close(); 
    }
    else {
        cout << "Error: The input file '" << importFileName << "' was not found in the current directory or could not be opened." << endl;
    }
}


void sortProcessesByArrivalTime(vector<Process> &processes){
    
    //Repetimos la comprobacion hasta que estemos seguros que esta totalmente ordenado
    for(int i = 1; i < (int)processes.size(); i++){
        //Recorremos el vector
        for(int j = 0; j < (int)processes.size() - 1; j++){   //Como compara parejas el último no hace falta analizarlo
            if(processes[j].arrivalTime > processes[j+1].arrivalTime){
                //Los valores mas grandes los vamos trasladando al final del vector
                swap(processes[j], (processes[j+1]));
            }
        }
    }
}

void insertMemoryHoleAfterIndex(CPU &processor, int holeIndex, int remainingMemoryAmount){
    Process newHole;
    newHole.name = "hole";
    newHole.arrivalTime = 0;
    newHole.remainingTimeUnits = 0;
    newHole.executionTimeUnits = 0;
    newHole.memoryRequirement = remainingMemoryAmount;
    processor.runningProcesses.push_back(newHole);
    
    for(int i = processor.runningProcesses.size() - 1 ; i > holeIndex + 1; i--){		
        swap(processor.runningProcesses[i], processor.runningProcesses[i-1]);
    }
}


void decreaseProcessesTimeUnitsAndConvertFinishedIntoHoles(vector<Process> &runningProcesses){
    
    //Recorremos el vector de processes que se estan ejecutando A LA INVERSA
    for(int k = runningProcesses.size() - 1; k >= 0 ; k--){
        
        //Holes must have unlimited lifetime
        if(runningProcesses[k].name.compare("hole") != 0)
        runningProcesses[k].remainingTimeUnits--;
        
        //If a process finishes
        if(runningProcesses[k].remainingTimeUnits == 0){    	
            //it becomes a hole
            runningProcesses[k].name = "hole"; 
        }   
    }
}

bool shouldContinueSimulation(vector<Process> queue, vector<Process> runningProcesses, int totalMemory){
    
    bool hasActiveProcesses = false;
    
    //Si la queue no tiene processes retenidos...
    if(queue.empty()){
        
        //Cuentas si te quedan processes vivos
        for(int i = 0; i < (int)runningProcesses.size() && hasActiveProcesses == false; i++){
            if(runningProcesses[i].remainingTimeUnits != 0)
            hasActiveProcesses = true;
        }
        
        //Always at least we will have a 'hole' process with remainingTimeUnits at 0
        if(runningProcesses[0].name.compare("hole") == 0 && runningProcesses[0].memoryRequirement == (unsigned int) totalMemory)
        hasActiveProcesses = false;
    }    
    
    else{
        //Cuentas si tienes cosas en queue
        hasActiveProcesses = true;
    }
    
    return hasActiveProcesses;
}

void mergeAdjacentMemoryHoles(vector<Process> &runningProcesses){
    
    for(int i = (int) runningProcesses.size() -1; i >= 0; i--){
        //Check if it can be merged with any other adjacent hole
        //NOTE: keep in mind that we cannot access position k=size
        if(i < (int) runningProcesses.size()  - 1 &&  runningProcesses[i].name.compare("hole") == 0 && runningProcesses[i+1].name.compare("hole") == 0){
            runningProcesses[i].memoryRequirement += runningProcesses[i+1].memoryRequirement;
            //Remove one of the redundant holes
            runningProcesses.erase(runningProcesses.begin() + i + 1);
        }
    }             
}

void moveProcessFromQueueToCPU(CPU &processor, bool &wasProcessMoved){
    int memoryHoleIndex = -1;
    
    // Check if there's a suitable memory hole for the first process in the queue
    for(int i=0; i < (int) processor.runningProcesses.size() && wasProcessMoved == false; i++){
        
        //If the first element of the queue fits in a memory hole
        if(processor.runningProcesses[i].name.compare("hole") == 0 && processor.queue[0].memoryRequirement <= processor.runningProcesses[i].memoryRequirement){
            
            memoryHoleIndex = i;
            int remainingMemory = processor.runningProcesses[i].memoryRequirement - processor.queue[0].memoryRequirement;
            
            //If it fits exactly, simply swap the values
            if( remainingMemory == 0){
                swap(processor.runningProcesses[i], processor.queue[0]);
            }
            
            //Otherwise, we need to create a new hole with the remaining memory
            else{
                //Shift all processes one position to the right and create the remaining hole
                insertMemoryHoleAfterIndex(processor, i, remainingMemory);
                //Insert the process
                swap(processor.runningProcesses[i], processor.queue[0]);
                //Merge adjacent holes if any consecutive holes exist
                mergeAdjacentMemoryHoles(processor.runningProcesses);
            }
            
            //Remove the process from the queue
            processor.queue.erase(processor.queue.begin());	
            wasProcessMoved = true;
        }
    }
    
    if(memoryHoleIndex == -1)
    wasProcessMoved = false;
}

void allocateProcessToCPU(vector<Process> &processes, CPU &processor){
    
    int memoryHoleIndex = -1;
    
    // Check if there's a suitable memory hole for the first process
    for(int i=0; i < (int) processor.runningProcesses.size(); i++){
        
        //If the first element of the processes vector fits in a memory hole
        if(processor.runningProcesses[i].name.compare("hole") == 0 && processes[0].memoryRequirement <= processor.runningProcesses[i].memoryRequirement){
            
            memoryHoleIndex = i;
            int remainingMemory = processor.runningProcesses[i].memoryRequirement - processes[0].memoryRequirement;
            
            //If it fits exactly, simply swap the values
            if( remainingMemory == 0){
                swap(processor.runningProcesses[i], processes[0]);
            }
            
            //Otherwise, we need to create a new hole with the remaining memory
            else{
                //Shift all processes one position to the right and create the remaining hole
                insertMemoryHoleAfterIndex(processor, i, remainingMemory);
                //Insert the process
                swap(processor.runningProcesses[i], processes[0]);
                //Merge adjacent holes if any consecutive holes exist
                mergeAdjacentMemoryHoles(processor.runningProcesses);
            }	
        }
    }
    
    if(memoryHoleIndex == -1){
        //If it doesn't fit in any hole, add to queue
        processor.queue.push_back(processes[0]);        
    }
    
    processes.erase(processes.begin()); 
}

void allocateProcessesUsingFirstFit(int totalMemory, string importFileName, string exportFileName){
    vector<Process> processes;
    
    loadProcessesFromFile(importFileName, processes);
    
    sortProcessesByArrivalTime(processes);
    
    //Initialize a processor responsible for executing the processes
    CPU processor;
    processor.totalMemory = (unsigned int) totalMemory;
    
    //Initialize the processor with a hole indicated by parameter
    Process initialization;
    initialization.name = "hole";
    initialization.arrivalTime = 0;
    initialization.remainingTimeUnits = 0;
    initialization.executionTimeUnits = 0;
    initialization.memoryRequirement = totalMemory;
    
    processor.runningProcesses.push_back(initialization);
    
    // true -> continue with the next time instant
    // false -> stop execution
    bool shouldContinueExecution = true;
    
    ofstream file_exported;
    
    file_exported.open(exportFileName.c_str(), ios::out);
    
    if(file_exported.is_open()){
        
        //Controls the time instants
        for(int currentTime = 1; shouldContinueExecution == true || !processes.empty(); currentTime++){
            
            //Time instant printed to terminal and file
            cout << currentTime << " ";
            file_exported << currentTime << " ";
            
            bool wasProcessRemoved;
            
            do{
                wasProcessRemoved = false;
                
                //If it doesn't fit even when the processor is empty, remove it
                if( !processor.queue.empty() && (processor.queue[0].memoryRequirement > processor.totalMemory)){
                    processor.queue.erase(processor.queue.begin());
                    wasProcessRemoved = true;
                }
                
                //If it could potentially fit...
                else if (!processor.queue.empty()){
                    moveProcessFromQueueToCPU(processor, wasProcessRemoved);
                }
                
            }while(wasProcessRemoved == true);
            
            
            //Relocate the original processes
            while(!processes.empty() && (int)processes[0].arrivalTime == currentTime){		
                allocateProcessToCPU(processes, processor);
            }		
            
            //We need to take into account the memory positions occupied by previous processes
            int startPosition = 0;
            
            //Print running processes at this time instant in the processor both to terminal and file
            for(int i = 0; i < (int)processor.runningProcesses.size(); i++){
                
                cout << "[" << startPosition << " " 
                << processor.runningProcesses[i].name << " " 
                << processor.runningProcesses[i].memoryRequirement << "] ";
                
                file_exported << "[" << startPosition << " " 
                << processor.runningProcesses[i].name << " " 
                << processor.runningProcesses[i].memoryRequirement << "] ";
                
                startPosition += processor.runningProcesses[i].memoryRequirement;
            }  
            
            cout << endl;
            file_exported << endl;
            
            shouldContinueExecution = shouldContinueSimulation(processor.queue, processor.runningProcesses, totalMemory);
            
            decreaseProcessesTimeUnitsAndConvertFinishedIntoHoles(processor.runningProcesses);
            mergeAdjacentMemoryHoles(processor.runningProcesses);
        }
        
        file_exported.close();
    }
    else
    cout << "The output file could not be created" << endl;
    
    cout << "Algorithm finished" << endl;
}

void allocateProcessesUsingBestFit(int totalMemory, string importFileName, string exportFileName){
    vector<Process> processes;
    
    loadProcessesFromFile(importFileName, processes);
    
    sortProcessesByArrivalTime(processes);
    
    //Initialize a processor responsible for executing the processes
    CPU processor;
    processor.totalMemory = (unsigned int) totalMemory;
    
    //Initialize the processor with a hole indicated by parameter
    Process initialization;
    initialization.name = "hole";
    initialization.arrivalTime = 0;
    initialization.remainingTimeUnits = 0;
    initialization.executionTimeUnits = 0;
    initialization.memoryRequirement = totalMemory;
    
    processor.runningProcesses.push_back(initialization);
    
    // true -> continue with the next time instant
    // false -> stop execution
    bool shouldContinueExecution = true;
    
    ofstream file_exported;
    
    file_exported.open(exportFileName.c_str(), ios::out);
    
    if(file_exported.is_open()){
        
        //Controls the time instants
        for(int currentTime = 1; shouldContinueExecution == true || !processes.empty(); currentTime++){
            
            //Time instant printed to terminal and file
            cout << currentTime << " ";
            file_exported << currentTime << " ";
            
            bool wasProcessRemoved;
            
            do{
                wasProcessRemoved = false;
                
                //If it doesn't fit even when the processor is empty, remove it
                if( !processor.queue.empty() && (processor.queue[0].memoryRequirement > processor.totalMemory)){
                    processor.queue.erase(processor.queue.begin());
                    wasProcessRemoved = true;
                }
                
                //If it could potentially fit...
                else if (!processor.queue.empty()){
                    moveProcessFromQueueToCPU(processor, wasProcessRemoved);
            }
            
        }while(wasProcessRemoved == true);
        
        
        //Relocate the original processes
        while(!processes.empty() && (int)processes[0].arrivalTime == currentTime){		
            int bestHoleIndex = -1;
            int memoryDifference = -1;
            
            // Check if there's a good hole for the 1st process in the queue
            for(int i=0; i < (int) processor.runningProcesses.size(); i++){
                
                //If the first element of the processes vector fits in a hole
                if(processor.runningProcesses[i].name.compare("hole") == 0 && processes[0].memoryRequirement <= processor.runningProcesses[i].memoryRequirement){
                    if(memoryDifference > processor.runningProcesses[i].memoryRequirement - processes[0].memoryRequirement || memoryDifference == -1){
                        bestHoleIndex = i;
                        memoryDifference = processor.runningProcesses[i].memoryRequirement - processes[0].memoryRequirement;	
                    }   		
                }
            }
            
            //If it doesn't fit in any hole
            if(bestHoleIndex == -1){
                //If it doesn't fit in any hole, add to queue
                processor.queue.push_back(processes[0]);        
            }
            else{
                //If it fits exactly, simply swap the values
                if( memoryDifference == 0){
                    swap(processor.runningProcesses[bestHoleIndex], processes[0]);
                }
                
                //Otherwise we need to create a new hole with the remaining memory
                else{
                    //Move all processes one position to the right and create the remaining hole
                    insertMemoryHoleAfterIndex(processor, bestHoleIndex, memoryDifference);
                    //Insert the process that fits
                    swap(processor.runningProcesses[bestHoleIndex], processes[0]);
                    //TODO hole checker: if there are several consecutive holes we should merge them 
                    mergeAdjacentMemoryHoles(processor.runningProcesses);
                }	
            }
            
            processes.erase(processes.begin()); 			    	
        }		
        
        //We need to take into account the memory positions occupied by previous processes
        int startPosition = 0;
        
        //Print running processes at this time instant in the processor both to terminal and file
        for(int i = 0; i < (int)processor.runningProcesses.size(); i++){
            
            cout << "[" << startPosition << " " 
            << processor.runningProcesses[i].name << " " 
            << processor.runningProcesses[i].memoryRequirement << "] ";
            
            file_exported << "[" << startPosition << " " 
            << processor.runningProcesses[i].name << " " 
            << processor.runningProcesses[i].memoryRequirement << "] ";
            
            startPosition += processor.runningProcesses[i].memoryRequirement;
        }       
        cout << endl;
        file_exported << endl;
        
        shouldContinueExecution = shouldContinueSimulation(processor.queue, processor.runningProcesses, totalMemory);
        decreaseProcessesTimeUnitsAndConvertFinishedIntoHoles(processor.runningProcesses);
        mergeAdjacentMemoryHoles(processor.runningProcesses);
        
    }
    
    file_exported.close();
    }
    else
        cout << "The output file could not be created" << endl;
    
    
    cout << "Algorithm finished" << endl;

}

int argumentValidation(int argc, char *argv[], string &importFileName, string &exportFileName, int &totalMemory, bool &useFirstFitAlgorithm, bool &useBestFitAlgorithm){
    
    int error = -1;

    //Loop through the arguments
    for(int i = 1; i < argc && error == -1; i++){
        switch(i){
            //First argument -> imported file name
            case 1:
                importFileName = argv[i];             
            break;

            //Second argument -> exported file name
            case 2:
                exportFileName = argv[i];
            break;
            
            //Third argument -> total memory size
            case 3:
                if(atoi(argv[i]) < 0)
                    error = 1;

                totalMemory = atoi(argv[i]);
            break;

            //Fourth argument -> selected algorithm
            case 4:
                //First fit algorithm
                if(strcmp(argv[i], "-f") == 0)
                    useFirstFitAlgorithm = true;

                //Best fit algorithm
                else if(strcmp(argv[i], "-b") == 0)
                    useBestFitAlgorithm = true;

                else
                    error = 2;
            break;
        }
    }
    return error;
}

int main(int argc, char *argv[]){
        
        string importFileName = "";
        string exportFileName = "";
        int totalMemory = 0;
        bool useFirstFitAlgorithm = false, useBestFitAlgorithm = false;
        
        //ARGUMENT CONTROL
        if(argc !=5){
            cout << "You have entered the arguments incorrectly." << endl;
            
        }
        else{
            int error = argumentValidation(argc, argv, importFileName, exportFileName, totalMemory, useFirstFitAlgorithm, useBestFitAlgorithm);
            //If errors are detected in the arguments, errors are shown and the program is terminated
            if(error == -1){
                cout << "Selected input file: " << importFileName << endl 
                << "Selected dump file: " << exportFileName << endl
                << "Maximum processor capacity: " << totalMemory << endl
                << "=========================================================" << endl;
                
                if(useFirstFitAlgorithm == true){
                    cout << "You have selected the first fit algorithm" << endl;
                    allocateProcessesUsingFirstFit(totalMemory, importFileName, exportFileName);
                }
                else{
                    cout << "You have selected the best fit algorithm" << endl;
                    allocateProcessesUsingBestFit(totalMemory, importFileName, exportFileName);
                }
            }
            
            else if(error == 1)
            cout << "The memory size entered as argument is negative." << endl;
            
            else if(error == 2)
            cout << "The algorithm command entered is incorrect." << endl;
            
            else
            cout << "Unexpected errors have occurred with the arguments" << endl;
            
        }
        
        return 0;
    }
    