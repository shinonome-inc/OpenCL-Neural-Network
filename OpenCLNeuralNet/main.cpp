#include "include.h"
#include "neuralnet.h"

//Reads a .cl file and creates a program from the .cl file
//Throws exceptions if there are build errors and logs the errors
cl::Program createProgram(cl::Context &context, std::string fname, const std::string params = "") 
{
    cl::Program::Sources sources;
    vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES> ();
    
    std::string sourceCode = getFileContents(fname.c_str());
    sources.insert(sources.end(), std::make_pair(sourceCode.c_str(),
        sourceCode.length()));
    cl::Program* program = new cl::Program(context,sources);

    try
    {
        (*program).build(devices, params.c_str());
    }
    catch (cl::Error e)
    {
        cout << "Compilation build error log: " << endl <<
            (*program).getBuildInfo <CL_PROGRAM_BUILD_LOG> (devices [0]) << endl;
    }

    return (*program);
}

/*Tests the code for a fully connected neural net to see if it converges
 *It generates a two dimensional training set (x,y), where if x > y
 *the the class is 1, otherwise the class is 0
 *It creates a 100-150-200-100-1 fully connected neural network
 *(i.e. 100 nodes -> 150 nodes -> 200 nodes -> 100 nodes -> 1 output node)
 */
void testFullyConnectedNeuralNet()
{
    //Get the platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    //Create a context
    cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM,
        (cl_context_properties)(*platforms.begin())(), 0};
    cl::Context context = cl::Context(CL_DEVICE_TYPE_GPU, cps);

    //Create and build the program
    cl::Program fullyConnectedNeuralNetProgram;
    fullyConnectedNeuralNetProgram = createProgram(context, "fullyconnectedneuralnet.cl");

    cl::Program convolutionalNeuralNetProgram;
    convolutionalNeuralNetProgram = createProgram(context, "convolutionalneuralnet.cl");

    //Create the command queue from the first device in context
    cl::CommandQueue queue;
    queue = cl::CommandQueue(context, context.getInfo<CL_CONTEXT_DEVICES>()[0], CL_QUEUE_PROFILING_ENABLE);

    //Create the neural network as a vector of layers
    //We include the input layer in the netSpec, which means that we will have to perform some offsets
    cl_int netSpecArray[] = {2, 100, 150, 200, 100, 1};
    vector<cl_int> netSpec (netSpecArray, netSpecArray + sizeof(netSpecArray)/sizeof(int)); 

    //Need to allocate the net to the heap as neural nets can be extremely large and cause stack overflow errors
    FullyConnectedNeuralNet *myNet = new FullyConnectedNeuralNet; 
    (*myNet).createFullyConnectedNeuralNet(netSpec);
    (*myNet).createMemoryBuffersAndKernels(context, fullyConnectedNeuralNetProgram);

    //Test it
    vector<std::tuple<float*, int*> > testData = getTestData();
    (*myNet).calculateError(&testData, &queue);

    //Ok, now train the neural net
    int trainingIterations = 20;
    (*myNet).trainFullyConnectedNeuralNet(&testData, &queue, trainingIterations);

    (*myNet).calculateError(&testData, &queue);
    (*myNet).writeFullyConnectedNeuralNetToFile(queue);
    cout << "Finished!" << endl;

    int wait;
    cin >> wait;

}

int main()
{
    //Get the platforms
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    //Create a context
    cl_context_properties cps[3] = {CL_CONTEXT_PLATFORM,
        (cl_context_properties)(*platforms.begin())(), 0};
    cl::Context context = cl::Context(CL_DEVICE_TYPE_GPU, cps);

    //Create and build the program
    cl::Program fullyConnectedNeuralNetProgram;
    fullyConnectedNeuralNetProgram = createProgram(context, "fullyconnectedneuralnet.cl");

    cl::Program convolutionalNeuralNetProgram;
    convolutionalNeuralNetProgram = createProgram(context, "convolutionalneuralnet.cl");

    //Create the command queue from the first device in context
    cl::CommandQueue queue;
    queue = cl::CommandQueue(context, context.getInfo<CL_CONTEXT_DEVICES>()[0], CL_QUEUE_PROFILING_ENABLE);

    //Create the neural network as a vector of layers
    //We include the input layer in the netSpec, which means that we will have to perform some offsets
    cl_int netSpecArray[] = {2, 100, 150, 200, 100, 1};
    vector<cl_int> netSpec (netSpecArray, netSpecArray + sizeof(netSpecArray)/sizeof(int)); 

    //Need to allocate the net to the heap as neural nets can be extremely large and cause stack overflow errors
    FullyConnectedNeuralNet *myNet = new FullyConnectedNeuralNet; 
    (*myNet).createFullyConnectedNeuralNet(netSpec);
    (*myNet).createMemoryBuffersAndKernels(context, fullyConnectedNeuralNetProgram);

    //Test it
    vector<std::tuple<float*, int*> > testData = getTestData();
    (*myNet).calculateError(&testData, &queue);

    //Ok, now train the neural net
    int trainingIterations = 20;
    (*myNet).trainFullyConnectedNeuralNet(&testData, &queue, trainingIterations);

    (*myNet).calculateError(&testData, &queue);
    (*myNet).writeFullyConnectedNeuralNetToFile(queue);
    cout << "Finished!" << endl;

    int wait;
    cin >> wait;
    return EXIT_SUCCESS;
}