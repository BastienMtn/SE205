public class BoundedBufferMain {

    public static void main (String[] args) {
        BoundedBuffer buffer;

        // Check the arguments of the command line
        if (args.length != 1){
            System.out.println ("PROGRAM FILENAME");
            System.exit(1);
        }
        Utils.init(args[0]);
        
        int n_consumers= (int)Utils.nConsumers;
        int n_producers = (int)Utils.nProducers;

        Consumer consumers[] = new Consumer[n_consumers];
        Producer producers[] = new Producer[n_producers];
        
        // Create a buffer
        if (Utils.sem_impl == 0)
            buffer = new NatBoundedBuffer(Utils.bufferSize);
        else
            buffer = new SemBoundedBuffer(Utils.bufferSize);

        // Create producers and then consumers
        // Create producers
        for(int i=0; i<n_producers; i++){
            producers[i] = new Producer(i, buffer);
            producers[i].start();
        }
        // Create consumers
        for(int i=0; i<n_consumers; i++){
            consumers[i] = new Consumer(i, buffer);
            consumers[i].start();
        }

    }
}
