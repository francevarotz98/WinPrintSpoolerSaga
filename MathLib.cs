using System;
namespace MathLib{
	public class Methods{
		public Methods(){}
		
		public static int Sum(int a, int b){
			return a+b;
		}
		
		public static int Diff(int a, int b)
        {
			return a-b ;
        }
		
		
		public static void PromptShell(){

			System.Diagnostics.Process.Start("CMD.exe", "dir");
			//return 0;
		
		}
		
	}
}