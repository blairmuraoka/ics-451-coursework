import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.FileInputStream;
import java.net.Socket;

public class Client {
  public static void main(String[] args) throws Exception {
    Socket socket = new Socket("localhost", Integer.parseInt(args[0]));
    boolean autoflush = true;
    PrintWriter out = new PrintWriter(socket.getOutputStream(), autoflush);
    BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    // send an HTTP request to the web server
    out.println("GET / HTTP/1.1");
    out.println("Host: localhost:" + args[0]);
    out.println("Connection: Close");
    out.println();

    File file = new File("test2.txt");
    InputStream is = new FileInputStream(file);
    OutputStream ops = socket.getOutputStream();
    
    int count;
    byte[] bytes = new byte[8192];
    while ((count = is.read(bytes)) > 0) {
    	ops.write(bytes, 0, count);
    }

    // read the response
    boolean loop = true;
    StringBuilder sb = new StringBuilder(8096);
    while (loop) {
      if (in.ready()) {
        int i = 0;
        while (i != -1) {
          i = in.read();
          sb.append((char) i);
        }
        loop = false;
      }
    }
    System.out.println(sb.toString());
    socket.close();
  }
}