package lielas;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintStream;

public class ExceptionHandler {

	public static void HandleException(Exception e){
		e.printStackTrace();
		PrintStream ps = null;
		try{
			ps = new PrintStream(new File("log.txt"));
		}catch(FileNotFoundException fnfE){
			fnfE.printStackTrace(ps);
		}
	}
	
}
