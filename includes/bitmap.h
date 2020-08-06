#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>

using namespace std;

#ifndef _BITMAP_HPP
#define _BITMAP_HPP

// 
class BADHEADER : public exception {
	public:
		string msg;
};

class Pixel
{
	private:
		int _state;				//State of pixel (for Cellular Automata)
		int _nextstate;			//State of pixel for after rectify is called

		int _rgb[4];			//current rgb value as number between 0-255
		int _nextrgb[4];		//next rgb value (to be set upon next step)
		int _heldrgb[4];		//held rgb value (for Cellular Automata)

	public:
		Pixel() 
		{
			_state = 0;

			for (int i = 0; i < 4; i++) {
				_rgb[i] = 0;
				_nextrgb[i] = 0;
				_heldrgb[i] = 0;
			}
		}

		Pixel(int* rgb) 
		{
			for (int i = 0; i < 4; i++) {
				_rgb[i] = rgb[i];
				_heldrgb[i] = rgb[i];
				_nextrgb[i] = 0;
			}

			//Activates pixels with r and b values equal to 255
			if (_rgb[0] == 255 && _rgb[1] == 255)
				_state = 0;
			else
				_state = 2;
		}

		Pixel(Pixel* p) 
		{
			_state = p->getState();
			_nextstate = 0;

			int* rgb = p->getRGB();
			int* heldrgb = p->getHeldRGB();

			for (int i = 0; i < 4; i++) {
				_rgb[i] = rgb[i];
				_heldrgb[i] = heldrgb[i];
				_nextrgb[i] = 0;
			}
		}

		int  getState() 			{return _state;}
		void transition(int state) 	{_nextstate = state;}
		
		int* getRGB() 				{return _rgb;}
		int* getHeldRGB() 			{return _heldrgb;}

		void setRGB(int* rgb) 
		{
			for (int i = 0; i < 4; i++)
				_rgb[i] = rgb[i];
		}

		void setRGB(int rgb)
		{
			for (int i = 0; i < 4; i++)
				_rgb[i] = rgb;
		}

		void setNextRGB(int* nextrgb) 
		{
			for (int i = 0; i < 4; i++)
				_nextrgb[i] = nextrgb[i];
		}

		void setHeldRGB(int* heldrgb) 
		{
			for (int i = 0; i < 4; i++)
				_heldrgb[i] = heldrgb[i];
		}

		void rectify() 
		{
			_state = _nextstate;
			_nextstate = 0;

			for (int i = 0; i < 4; i++) {
				if (_state == 2)
					_rgb[i] = _heldrgb[i];
				else if (_state == 1)
					_rgb[i] = (int) (0.8 * ((float)_heldrgb[i]) + 0.2 * ((float)(255 - _heldrgb[i])));
				else
					_rgb[i] = 255 - _heldrgb[i];
			}
		}

		void rectifyRGB()
		{
			for (int i = 0; i < 4; i++) {
				_rgb[i] = _nextrgb[i];
			}
		}
};

class Bitmap
{
	private:
		friend istream& operator>>(istream& in, Bitmap& b);
		friend ostream& operator<<(ostream& out, const Bitmap& b);
		
		char header[139];
		int headerLength;
		
		int size[2];
		int depth;
		int maskRef[4];
		
		vector< vector<Pixel> > map;

	public:

		Bitmap() {}

		Bitmap(int type, int width, int height);

		~Bitmap() {
			for (int i = 0; i < size[0]; i++) 
				for (int j = 0; j < size[1]; j++) 
					map[i].pop_back();
		}
		
		//For use in overloading insertion operator
		void setHeader(char* p_header, int p_length) 
		{
			memset(header, '\0', 139);
			memcpy(header, p_header, p_length);	
			headerLength = p_length;
		}
		
		//For use in getting bitmap width and height
		int getSize(int n) {return size[n];}	

		//For use in getting bitmap color depth
		int getDepth() {return depth;}

		//For use in getting/setting pixel values
		Pixel& getPixel(int i, int j) {return map[i][j];}

		//For use in overloading extraction operator
		Pixel getConstPixel(int i, int j) const {return map[i][j];}
};

Bitmap::Bitmap(int type, int width, int height)
{
	size[0] = width;
	size[1] = height;

	/*

	for (int i = 0; i < size[0]; i++) {
	
		vector<Pixel> line;			
		
		for (int j = 0; j < size[1]; j++) {
			
			if (b.depth == 32) {	//Reads 32 bit pixel
				for (int k = 0; k < 4; k++) { 
					in.read((char*) &pixleData[k], 1);
					rgb[maskRef[k]] = pixleData[k];
				}	
			} else {				//Reads 24 bit pixel
				for (int k = 0; k < 3; k++) {
					in.read((char*) &pixleData[k], 1);
					rgb[k] = pixleData[k];
				}
			}

			Pixel* p = new Pixel(rgb);
			line.push_back(*p);
		}

		b.map.push_back(line);	
		*/
}

//Takes Bitmap object B, and outputs frame as vector of uint8_t
//	Stores each pixel's RGBA values consecutively
void getFrame(vector<uint8_t>& frame, Bitmap& b)
{
	frame.clear();

	for (int i = 0; i < b.getSize(0); i++) 
	{
		for (int j = 0; j < b.getSize(1); j++)
		{
			int* rgb = b.getPixel(i, j).getRGB();

			for (int k = 0; k < 4; k++)
				frame.push_back((uint8_t) rgb[k]);
		}		
	}
}

//Translates mask to an integer value corresponding to the "turned on" byte
//	MASK MUST BE OF TYPE ONE-HOT
int getMask(char* mask) 
{
	for (int i = 0; i < 4; i++) {
		if (mask[i] != '\0'){  
			return 4 - i; //takes into account endianness
		}
	}
	return 0;
}

//Reads in Bitmap file to object of class Bitmap
istream& operator>>(istream& in, Bitmap& b)
{
	BADHEADER e;
	char tag[2];
	uint32_t info[6];

	for (int i = 0; i < 6; i++)
		info[i] = 0;

	cout << "	istream Initiated..." << endl;

//First Header
	in >> tag[0] >> tag[1];				//Bitmap Tag		2 bytes - TOSS/CHECK
		if (strncmp(tag, "BM", 2) != 0)	throw e; 
	in.read((char*)&info[0], 4);		//Size of Bitmap	4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore(); 					//garbage 			4 bytes - TOSS	
	for (int i = 0; i < 4; i++)
		in.ignore();					//offset			4 bytes - TOSS

	cout << "	istream First Header Read..." << endl;

//Second Header	
	for (int i = 0; i < 4; i++)
		in.ignore();					//size of 2nd header	4 bytes - TOSS
	in.read((char*)&info[1], 4);		//Image width		4 bytes - KEEP
		b.size[1] = info[1];
	in.read((char*)&info[2], 4);		//Image height		4 bytes - KEEP
		b.size[0] = info[2];
	in.read((char*)&info[3], 2);		//# of color planes	2 bytes - TOSS/CHECK
		if (info[3] != 1) 				    throw e;	
	in.read((char*)&info[4], 2);		//Color depth		2 bytes - KEEP/CHECK
		if (info[4] != 24 && info[4] != 32) throw e;
		b.depth = info[4];
	in.read((char*)&info[5], 4);		//Compression		4 bytes - TOSS/CHECK
		if (info[5] != 0 && info[5] != 3)   throw e;
		if (info[5] == 0 && info[4] != 24)  throw e;
		if (info[5] == 3 && info[4] != 32)  throw e;	
	for (int i = 0; i < 4; i++)
		in.ignore();					//size of raw data	4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore();					//horz res 			4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore();					//vert res			4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore();					//color pallete		4 bytes - TOSS
	for (int i = 0; i < 4; i++)
		in.ignore();					//Important colors	4 bytes - TOSS

	cout << "	istream Second Header Read..." << endl;

//Masks - only included if color depth == 32
	char** masks;
	char maskOrder[4];
	int maskRef[4];

	if (b.depth == 32) {

		masks = new char*[4];
		for (int i = 0; i < 4; i++) 
			masks[i] = new char[4];

		in.read(masks[0], 4);			//Mask 1		4 bytes - KEEP
		in.read(masks[1], 4);			//Mask 2		4 bytes - KEEP
		in.read(masks[2], 4);			//Mask 3		4 bytes - KEEP
		in.read(masks[3], 4);			//Mask 4		4 bytes - KEEP
		for (int i = 0; i < 4; i++)
			in >> maskOrder[i];			//Mask Order		4 bytes - KEEP
		for (int i = 0; i < 64; i++)
			in.ignore();				//Color Space Info	64bytes - TOSS

		//Set maskRef to deal with bit masking input
		//Set b.maskRef to deal with bit masking output
		for (int i = 0; i < 4; i++) {
			     if (maskOrder[i] == 'R') {
				maskRef[i] = getMask(masks[0]);
				b.maskRef[maskRef[i]] = i;
			}
			else if (maskOrder[i] == 'G') {
				maskRef[i] = getMask(masks[1]);
				b.maskRef[maskRef[i]] = i;
			}
			else if (maskOrder[i] == 'B') {
				maskRef[i] = getMask(masks[2]);
				b.maskRef[maskRef[i]] = i;
			}
			else if (maskOrder[i] == 's') {
				maskRef[i] = getMask(masks[3]);
				b.maskRef[maskRef[i]] = i;
			}
			else 
				throw e;
		}
	}

//Store header
	int length = in.tellg();					//store position in input stream as 'length'	
	char* header = new char[length];  			//create header int of size 'length'
	in.seekg(0, in.beg);						//retrace steps through input stream
	in.read(header, length);					//read in data to 'length'
	b.setHeader(header, length);				//store header in object of class Bitmap

	cout << "	istream Header Saved..." << endl;

//Store pixel information
	uint32_t pixleData[4];
	int rgb[4];

	for (int i = 0; i < b.size[0]; i++) {
	
		vector<Pixel> line;			
		
		for (int j = 0; j < b.size[1]; j++) {
			
			if (b.depth == 32) {	//Reads 32 bit pixel
				for (int k = 0; k < 4; k++) { 
					in.read((char*) &pixleData[k], 1);
					rgb[maskRef[k]] = pixleData[k];
				}	
			} else {				//Reads 24 bit pixel
				for (int k = 0; k < 3; k++) {
					in.read((char*) &pixleData[k], 1);
					rgb[k] = pixleData[k];
				}
			}

			Pixel* p = new Pixel(rgb);
			line.push_back(*p);
		}

		b.map.push_back(line);	

		//Skip input padding, if applicable
		if (b.depth == 24) {
			for (int j = 0; j < (b.size[1] % 4); j++)
				in.ignore();	
		}
	}

	return in;
}

//Converts integer to binary char array
char* getBinary(int integer) {
	char* binary = new char[1];	
	binary[0] = integer & 0xff;
	return binary;
}

ostream& operator<<(ostream& out, const Bitmap& b) {
	//Write bitmap back to file

	char* binary;
	int* rgb;
	int alpha;

	out.write(b.header, b.headerLength);

	for (int i = 0; i < b.size[0]; i++) {
		for (int j = 0; j < b.size[1]; j++) {

			rgb = b.getConstPixel(i, j).getRGB();

			if (b.depth == 32) {		//Writes 32 bit pixel
				for (int k = 0; k < 4; k++) {
					binary = getBinary(rgb[b.maskRef[k]]);
					out.write(binary, 1);
					delete[] binary;
				}
			} else {					//Writes 24 bit pixel
				for (int k = 0; k < 3; k++) {
					binary = getBinary(rgb[k]);
					out.write(binary, 1);
					delete[] binary;
				}
			}
		}	
		
		//Pad output with zeros, if applicable
		if (b.depth == 24) {
			int padding = b.size[1] % 4;

			if (padding) {
				binary = new char[padding];
				memset(binary, '\0', padding);
				out.write(binary, padding);
				delete[] binary;
			}	
		}
	}

	return out;
}

#endif