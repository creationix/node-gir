/* This Example is forked from https://github.com/antono/vala-object */
namespace ValaObject {
	public void say_hello_to(string lang)
	{
		print(@"I love You, $lang!!!\n");
		print("-- Vala\n\n");
	}
	public class ValaClass : Object {
		public string name = "Vala Class";

		public ValaClass(){
			print("I'm the Constructor"); // FIXME Constructor dosn't work
		}
		
		public string append_to_name(string suffix) {
			return @"$name $suffix";
		}
	}
}
