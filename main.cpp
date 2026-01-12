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

struct Metrics{
    int totalProcesses;
    int successfulAllocations;
    int allocationFailures;
    int totalSimulationTime;
    int totalMemoryTimeUsed; // Sum of memory usage over time for average calculation
    int peakMemoryUsage;
    int totalWaitingTime;
    int processesCompleted;
    int minQueueLength;
    int maxQueueLength;
    vector<int> memoryUtilizationOverTime;
    vector<int> holeCountOverTime;
    vector<int> holeSizesAtEnd;
    vector<int> fragmentationOverTime; // Total size of holes at each time step
};

void initializeMetrics(Metrics &metrics) {
    metrics.totalProcesses = 0;
    metrics.successfulAllocations = 0;
    metrics.allocationFailures = 0;
    metrics.totalSimulationTime = 0;
    metrics.totalMemoryTimeUsed = 0;
    metrics.peakMemoryUsage = 0;
    metrics.totalWaitingTime = 0;
    metrics.processesCompleted = 0;
    metrics.minQueueLength = 999999; // Initialize to a large number
    metrics.maxQueueLength = 0;
    metrics.memoryUtilizationOverTime.clear();
    metrics.holeCountOverTime.clear();
    metrics.holeSizesAtEnd.clear();
    metrics.fragmentationOverTime.clear();
}

void updateMetrics(Metrics &metrics, const CPU &processor, int currentTime, int totalMemory) {
    // Calculate current memory usage
    int currentMemoryUsage = 0;
    int holeCount = 0;
    int totalFragmentation = 0;
    
    for(int i = 0; i < (int)processor.runningProcesses.size(); i++) {
        if(processor.runningProcesses[i].name.compare("hole") != 0) {
            currentMemoryUsage += processor.runningProcesses[i].memoryRequirement;
        } else {
            holeCount++;
            totalFragmentation += processor.runningProcesses[i].memoryRequirement;
        }
    }
    
    // Update peak memory usage
    if(currentMemoryUsage > metrics.peakMemoryUsage) {
        metrics.peakMemoryUsage = currentMemoryUsage;
    }
    
    // Track queue length
    int currentQueueLength = processor.queue.size();
    if(currentQueueLength < metrics.minQueueLength) {
        metrics.minQueueLength = currentQueueLength;
    }
    if(currentQueueLength > metrics.maxQueueLength) {
        metrics.maxQueueLength = currentQueueLength;
    }
    
    // Store utilization and hole count for this time step
    metrics.memoryUtilizationOverTime.push_back(currentMemoryUsage);
    metrics.holeCountOverTime.push_back(holeCount);
    metrics.fragmentationOverTime.push_back(totalFragmentation);
    metrics.totalMemoryTimeUsed += currentMemoryUsage;
    
    // Calculate waiting time for processes in queue
    for(int i = 0; i < (int)processor.queue.size(); i++) {
        metrics.totalWaitingTime += (currentTime - processor.queue[i].arrivalTime);
    }
}

void finalizeMetrics(Metrics &metrics, const CPU &processor, int totalMemory) {
    // Collect final hole sizes
    for(int i = 0; i < (int)processor.runningProcesses.size(); i++) {
        if(processor.runningProcesses[i].name.compare("hole") == 0) {
            metrics.holeSizesAtEnd.push_back(processor.runningProcesses[i].memoryRequirement);
        }
    }
}

void printMetrics(const Metrics &metrics, int totalMemory, const string &algorithmName) {
    cout << "\n========== " << algorithmName << " ALGORITHM METRICS ==========" << endl;
    cout << "Total Processes: " << metrics.totalProcesses << endl;
    cout << "Successful Allocations: " << metrics.successfulAllocations << endl;
    cout << "Allocation Failures: " << metrics.allocationFailures << endl;
    
    if(metrics.totalProcesses > 0) {
        double successRate = (double)metrics.successfulAllocations / metrics.totalProcesses * 100;
        cout << "Allocation Success Rate: " << successRate << "%" << endl;
    }
    
    cout << "Processes Completed: " << metrics.processesCompleted << endl;
    cout << "Total Simulation Time: " << metrics.totalSimulationTime << endl;
    
    if(metrics.totalSimulationTime > 0) {
        double avgUtilization = (double)metrics.totalMemoryTimeUsed / (metrics.totalSimulationTime * totalMemory) * 100;
        cout << "Average Memory Utilization: " << avgUtilization << "%" << endl;
        
        double throughput = (double)metrics.processesCompleted / metrics.totalSimulationTime;
        cout << "Throughput (processes/time unit): " << throughput << endl;
    }
    
    cout << "Peak Memory Usage: " << metrics.peakMemoryUsage << "/" << totalMemory 
         << " (" << (double)metrics.peakMemoryUsage / totalMemory * 100 << "%)" << endl;
    
    if(metrics.successfulAllocations > 0) {
        double avgWaitingTime = (double)metrics.totalWaitingTime / metrics.successfulAllocations;
        cout << "Average Waiting Time: " << avgWaitingTime << " time units" << endl;
    }
    
    // Display queue length metrics
    cout << "Minimum Queue Length: " << metrics.minQueueLength << endl;
    cout << "Maximum Queue Length: " << metrics.maxQueueLength << endl;
    
    // Calculate and display average fragmentation over time
    if(!metrics.fragmentationOverTime.empty()) {
        int totalFragmentation = 0;
        int maxFragmentation = 0;
        for(int i = 0; i < (int)metrics.fragmentationOverTime.size(); i++) {
            totalFragmentation += metrics.fragmentationOverTime[i];
            if(metrics.fragmentationOverTime[i] > maxFragmentation) {
                maxFragmentation = metrics.fragmentationOverTime[i];
            }
        }
        double avgFragmentation = (double)totalFragmentation / metrics.fragmentationOverTime.size();
        cout << "Average Fragmentation (over time): " << avgFragmentation << " (" 
             << (avgFragmentation / totalMemory * 100) << "% of total memory)" << endl;
        cout << "Peak Fragmentation: " << maxFragmentation << " (" 
             << ((double)maxFragmentation / totalMemory * 100) << "% of total memory)" << endl;
    }
    
    cout << "Final Number of Memory Holes: " << metrics.holeSizesAtEnd.size() << endl;
    
    if(!metrics.holeSizesAtEnd.empty()) {
        int totalHoleSize = 0;
        int minHole = metrics.holeSizesAtEnd[0];
        int maxHole = metrics.holeSizesAtEnd[0];
        
        for(int i = 0; i < (int)metrics.holeSizesAtEnd.size(); i++) {
            totalHoleSize += metrics.holeSizesAtEnd[i];
            if(metrics.holeSizesAtEnd[i] < minHole) minHole = metrics.holeSizesAtEnd[i];
            if(metrics.holeSizesAtEnd[i] > maxHole) maxHole = metrics.holeSizesAtEnd[i];
        }
        
        double avgHoleSize = (double)totalHoleSize / metrics.holeSizesAtEnd.size();
        cout << "Average Hole Size: " << avgHoleSize << endl;
        cout << "Smallest Hole: " << minHole << endl;
        cout << "Largest Hole: " << maxHole << endl;
        cout << "Total Fragmented Memory: " << totalHoleSize << "/" << totalMemory 
             << " (" << (double)totalHoleSize / totalMemory * 100 << "%)" << endl;
    }
    
    cout << "=========================================================\n" << endl;
}

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


void decreaseProcessesTimeUnitsAndConvertFinishedIntoHoles(vector<Process> &runningProcesses, Metrics &metrics){
    
    //Recorremos el vector de processes que se estan ejecutando A LA INVERSA
    for(int k = runningProcesses.size() - 1; k >= 0 ; k--){
        
        //Holes must have unlimited lifetime
        if(runningProcesses[k].name.compare("hole") != 0)
        runningProcesses[k].remainingTimeUnits--;
        
        //If a process finishes
        if(runningProcesses[k].remainingTimeUnits == 0 && runningProcesses[k].name.compare("hole") != 0){		
            //it becomes a hole
            runningProcesses[k].name = "hole";
            metrics.processesCompleted++;
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

void moveProcessFromQueueToCPU(CPU &processor, bool &wasProcessMoved, Metrics &metrics){
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
            metrics.successfulAllocations++;
            metrics.allocationFailures--; // Compensate for initial failure count
        }
    }
    
    if(memoryHoleIndex == -1)
    wasProcessMoved = false;
}

void allocateProcessToCPU(vector<Process> &processes, CPU &processor, Metrics &metrics){
    
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
            metrics.successfulAllocations++;
            break;
        }
    }
    
    if(memoryHoleIndex == -1){
        //If it doesn't fit in any hole, add to queue
        processor.queue.push_back(processes[0]);
        metrics.allocationFailures++;
    }
    
    processes.erase(processes.begin()); 
}

void allocateProcessesUsingFirstFit(int totalMemory, string importFileName, string exportFileName){
    vector<Process> processes;
    
    loadProcessesFromFile(importFileName, processes);
    
    sortProcessesByArrivalTime(processes);
    
    // Initialize metrics
    Metrics metrics;
    initializeMetrics(metrics);
    metrics.totalProcesses = processes.size();
    
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
            
            //Time instant printed to file only
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
                    moveProcessFromQueueToCPU(processor, wasProcessRemoved, metrics);
                }
                
            }while(wasProcessRemoved == true);
            
            
            //Relocate the original processes
            while(!processes.empty() && (int)processes[0].arrivalTime == currentTime){		
                allocateProcessToCPU(processes, processor, metrics);
            }		
            
            //We need to take into account the memory positions occupied by previous processes
            int startPosition = 0;
            
            //Print running processes at this time instant to file only
            for(int i = 0; i < (int)processor.runningProcesses.size(); i++){
                
                file_exported << "[" << startPosition << " " 
                << processor.runningProcesses[i].name << " " 
                << processor.runningProcesses[i].memoryRequirement << "] ";
                
                startPosition += processor.runningProcesses[i].memoryRequirement;
            }  
            
            file_exported << endl;
            
            // Update metrics for this time step
            updateMetrics(metrics, processor, currentTime, totalMemory);
            
            shouldContinueExecution = shouldContinueSimulation(processor.queue, processor.runningProcesses, totalMemory);
            
            decreaseProcessesTimeUnitsAndConvertFinishedIntoHoles(processor.runningProcesses, metrics);
            mergeAdjacentMemoryHoles(processor.runningProcesses);
            
            metrics.totalSimulationTime = currentTime;
        }
        
        file_exported.close();
    }
    else
    cout << "The output file could not be created" << endl;
    
    // Finalize and print metrics
    finalizeMetrics(metrics, processor, totalMemory);
    printMetrics(metrics, totalMemory, "FIRST FIT");
    
    cout << "Algorithm finished" << endl;
}

void allocateProcessesUsingBestFit(int totalMemory, string importFileName, string exportFileName){
    vector<Process> processes;
    
    loadProcessesFromFile(importFileName, processes);
    
    sortProcessesByArrivalTime(processes);
    
    // Initialize metrics
    Metrics metrics;
    initializeMetrics(metrics);
    metrics.totalProcesses = processes.size();
    
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
            
            //Time instant printed to file only
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
                    moveProcessFromQueueToCPU(processor, wasProcessRemoved, metrics);
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
                metrics.allocationFailures++;
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
                metrics.successfulAllocations++;
            }
            
            processes.erase(processes.begin()); 			    	
        }		
        
        //We need to take into account the memory positions occupied by previous processes
        int startPosition = 0;
        
        //Print running processes at this time instant to file only
        for(int i = 0; i < (int)processor.runningProcesses.size(); i++){
            
            file_exported << "[" << startPosition << " " 
            << processor.runningProcesses[i].name << " " 
            << processor.runningProcesses[i].memoryRequirement << "] ";
            
            startPosition += processor.runningProcesses[i].memoryRequirement;
        }       
        file_exported << endl;
        
        // Update metrics for this time step
        updateMetrics(metrics, processor, currentTime, totalMemory);
        
        shouldContinueExecution = shouldContinueSimulation(processor.queue, processor.runningProcesses, totalMemory);
        decreaseProcessesTimeUnitsAndConvertFinishedIntoHoles(processor.runningProcesses, metrics);
        mergeAdjacentMemoryHoles(processor.runningProcesses);
        
        metrics.totalSimulationTime = currentTime;
        
    }
    
    file_exported.close();
    }
    else
        cout << "The output file could not be created" << endl;
    
    // Finalize and print metrics
    finalizeMetrics(metrics, processor, totalMemory);
    printMetrics(metrics, totalMemory, "BEST FIT");
    
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
    