import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
public class Server {

    public static void main(String args[] ) throws IOException {
        String s = args[0];
        ServerSocket server = new ServerSocket(Integer.parseInt(s));
        System.out.println("Listening for connection on port " + args[0] + " ....");
        while (true) {
            Socket clientSocket = server.accept();
            InputStreamReader isr =  new InputStreamReader(clientSocket.getInputStream());
            BufferedReader reader = new BufferedReader(isr);
            String line = reader.readLine();            
            while (!line.isEmpty()) {
                System.out.println(line);
                line = reader.readLine();
            }
        }
    }

}