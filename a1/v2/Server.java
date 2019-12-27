import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {

    public static void main(String args[] ) throws IOException {
        ServerSocket server = new ServerSocket(Integer.parseInt(args[0]));
        OutputStream out = null;
        System.out.println("Listening for connection on port " + args[0] + " ....");
    
        Socket clientSocket = server.accept();
        InputStreamReader isr =  new InputStreamReader(clientSocket.getInputStream());
        InputStream in = clientSocket.getInputStream();
        BufferedReader reader = new BufferedReader(isr);
        String line = reader.readLine();            
        while (!line.isEmpty()) {
            System.out.println(line);
            line = reader.readLine();
        }

        try {
            out = new FileOutputStream("server\\test.txt");
        } catch (IOException ex) {
            System.out.println("File not found");
        }

        byte[] bytes = new byte[8192];
        int count;
        while ((count = in.read(bytes)) > 0) {
            out.write(bytes, 0,  count);
        }

        out.close();
        reader.close();
        clientSocket.close();
        server.close();

    }

}