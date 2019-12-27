import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.BufferedOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;

public class Client {
  public static void main(String[] args) throws IOException {
    int count = 0;
    int bytesRead;
    Socket socket = null;
    FileOutputStream fos = null;
    BufferedOutputStream bos = null;

    try {
    	// Connects to server
        socket = new Socket("localhost", Integer.parseInt(args[0]));
    	System.out.println("Connecting...");

    	byte[] bytes = new byte[8192];
    	InputStream is = socket.getInputStream();

        // Download the file
    	fos = new FileOutputStream("test_download.txt");
    	bos = new BufferedOutputStream(fos);
    	bytesRead = is.read(bytes, 0, bytes.length);
    	count = bytesRead;

        while((bytesRead = is.read(bytes)) != -1) {
            count += bytesRead;
        }
      	bos.write(bytes, 0, count);
      	bos.flush();
    	bos.close();
    	is.close();
        System.out.println("File downloaded.");
    	socket.close();
    } catch(IOException e) {
        System.out.println("Error");
    }

  }
}