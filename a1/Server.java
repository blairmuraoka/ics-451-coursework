import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {

    public static void main(String args[] ) throws IOException {
        ServerSocket server = null;
        Socket socket = null;
        BufferedInputStream bis = null;
        FileInputStream fis = null;
        OutputStream os = null;

        try {
            server = new ServerSocket(Integer.parseInt(args[0]));
            
            System.out.println("Waiting for connection");
            try {
                socket = server.accept();
                System.out.println("Accepted connection: " + socket);

                // Send the file
                File file = new File("test.txt");
                byte[] bytes = new byte[(int)file.length()];
                fis = new FileInputStream(file);
                bis = new BufferedInputStream(fis);
                bis.read(bytes, 0, bytes.length);
                os = socket.getOutputStream();
                System.out.println("Sending test.txt (" + bytes.length + " bytes)");
                os.write(bytes, 0, bytes.length);
                os.flush();
                System.out.println("File transfer complete");
                socket.close();
            } catch(IOException e) {
                System.out.println("Error");
            } 

            server.close();

            

        } catch(IOException e) {
            System.out.println("Error");
        }
    }

}