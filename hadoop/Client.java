// the client for the hadoop google

import java.io.*;
import java.util.*;
import java.net.Socket;

public class Client{


    public static void main(String[] args){
        
        try{
            Scanner nsRead = new Scanner(new FileReader("../ns.txt"));
            boolean connected = false;
            // find the nameserver
            Socket nsSocket = null;
            while(nsRead.hasNextLine()){
                String nsLine = nsRead.nextLine();
                System.out.println("[INFO] trying ns at: " + nsLine);
                String[] nsIn = nsLine.split(":");
                try{
                    nsSocket = new Socket(nsIn[0], Integer.valueOf(nsIn[1]));
                    System.out.println("[INFO] connected to ns");
                    connected = true;
                    break;
                } catch (Exception nsE){
                    System.out.println("[INFO] trying next");
                    continue;
                }
            }
            nsRead.close();
            if(!connected){ // can't connect
                System.out.println("[FATAL] could not connect to ns");
                System.exit(-1);
            }
            String ipString = "";
            DataOutputStream nsOutStream = new DataOutputStream(nsSocket.getOutputStream());


            System.out.println("[DEBUG] created DataOutPutStream");
            System.out.println("[AHHH] please work");
            System.out.println("[AHHH] 1 = " + 1);
            nsOutStream.writeInt(1); // tell the server we're requesting workers
            System.out.println("[DEBUG] sent 1");
            nsOutStream.writeInt(1); // since this is hadoop, we only need to request 1
            System.out.println("[DEBUG] sent 1");
            DataInputStream  nsInStream  = new DataInputStream( nsSocket.getInputStream() );
            System.out.println("[DEBUG] created inStream");
            int workersToRecv = nsInStream.readInt();
            System.out.println("[INFO] we're getting " + workersToRecv + " workers");
            byte[] bytes = new byte[100];
            nsInStream.read(bytes);
            String workerString = new String(bytes);
            System.out.println("[INFO] here's our worker: " + workerString);
            nsSocket.close();

        } catch (Exception e){
            System.out.println("[FATAL] could not contact ns");
            System.out.println(e);
            System.exit(-1);
        }
        

    }

}

