#include "CImg.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace cimg_library;

float NB_VOX_IN_INTERVAL = 0;


//Display Width, Depth, Min and Max Value of an Image
void displayBasicValues(CImg<float> * img){
	cout<<"Width : "<<img->width()<<"\nHeight : "<<img->height()
	<<"\nDepth : "<<img->depth()
	<<"\nMin : "<<img->min()
	<<"\nMax : "<<img->max()<<endl;
}

void displayValue(CImg<float> * img, int x, int y, int z){
	cout << *img->data(x,y,z)<<endl;
}

CImg<float> MIP(CImg<float> in){
	CImg<float> out(in.width(),in.height());
	for(int i = 0;i<in.width();i++){
		for(int j = 0;j<in.height();j++){
			float max = in.get_crop(i,j,0,0,i,j,in.depth(),0).max();
			out(i,j) = max;
		}
	}
	return out;
}


CImg<float> AIP(CImg<float> in){
	CImg<float> out(in.width(),in.height());
	for(int i = 0;i<in.width();i++){
		for(int j = 0;j<in.height();j++){
			float mean = in.get_crop(i,j,0,0,i,j,in.depth(),0).mean();
			out(i,j) = mean;
		}
	}
	return out;
}

CImg<float> firstIP(CImg<float> in){
	CImg<float> out(in.width(),in.height());
	for(int i = 0;i<in.width();i++){
		for(int j = 0;j<in.height();j++){
			float value = 0;
			int depth = 0;
			while(value==0 && depth<in.depth()){
				value = in(i,j,depth);
				depth++;
			}
			out(i,j) = value;
		}
	}
	return out;
}

CImg<float> visuMIP(CImg<float> in,int min,int max){
	CImg<float> out(in.width(),in.height(),1,1,0);
	for(int z = 0;z<in.depth();z++){
		for(int i = 0;i<in.width();i++){
			for(int j = 0;j<in.height();j++){
				if(in(i,j,z)>=min && in(i,j,z)<=max){
					out(i,j) = in(i,j,z);
				}else if(out(i,j)<min || out(i,j)>max){
					out(i,j) = out(i,j)>in(i,j,z)? out(i,j):in(i,j,z);
				}
			}
		}
	}
	return out;
}

float countNbInInterval(CImg<float> in,int min,int max){
	CImg<int> hist = in.histogram(256,min,max);
	return  (float) hist.sum();
}

float povValue(CImg<float> projected,int min,int max){
	return (float) countNbInInterval(projected,min,max)/NB_VOX_IN_INTERVAL;
}

CImg<float> getBestRot(CImg<float> img,float pasX,float pasY,float pasMin,int min,int max){
	int center_x = img.width()/2;
	int center_y = img.height()/2;
	int center_z = img.depth()/2;

	CImg<float> projected = visuMIP(img,min,max);
	float value = povValue(projected,min,max);
	cout<<"Searching best direction to start ..."<<endl;

	CImg<float> rot = img;
	rot.rotate(1,0,0,pasX,center_x,center_y,center_z,1,0);
	projected = visuMIP(rot,min,max);
	float valueX1 = povValue(projected,min,max);

	rot = img;
	rot.rotate(1,0,0,-pasX,center_x,center_y,center_z,1,0);
	projected = visuMIP(rot,min,max);
	float valueX2 = povValue(projected,min,max);

	if(valueX1 < valueX2){
		pasX = -pasX;
	}

	rot = img;
	rot.rotate(0,1,0,pasY,center_x,center_y,center_z,1,0);
	projected = visuMIP(rot,min,max);
	float valueY1 = povValue(projected,min,max);

	rot = img;
	rot.rotate(0,1,0,-pasY,center_x,center_y,center_z,1,0);
	projected = visuMIP(rot,min,max);
	float valueY2 = povValue(projected,min,max);

	if(valueY1 < valueY2){
		pasY = -pasY;
	}

	cout<<"...best first direction found"<<endl;

	rot = img;
	float currentValue = 0;
	float totX = 0;
	float totY = 0;
	while(abs(pasX)>pasMin || abs(pasY)>pasMin){
		rot = img;

		if(abs(pasX)>pasMin){
			center_x = img.width()/2;
			center_y = img.height()/2;
			center_z = img.depth()/2;
			totX += pasX;
			rot.rotate(1,0,0,totX,center_x,center_y,center_z,1,0);
			projected = visuMIP(rot,min,max);
			currentValue = povValue(projected,min,max);
			if(currentValue < value){
				cout<<"Changing X direction"<<endl;
				pasX = (pasX*-1)/2;
			}
			value = currentValue;
		}

		if(abs(pasY)>pasMin){
			center_x = img.width()/2;
			center_y = img.height()/2;
			center_z = img.depth()/2;
			totY += pasY;
			rot.rotate(0,1,0,totY,center_x,center_y,center_z,1,0);
			projected = visuMIP(rot,min,max);
			currentValue = povValue(projected,min,max);
			if(currentValue < value){
				cout<<"Changing Y direction"<<endl;
				pasY = (pasY*-1)/ 2;
			}
			value = currentValue;
		}
	}
	return rot;
}

CImg<float> getWorstRot(CImg<float> img,float pasX,float pasY,float pasMin,int min,int max){
	int center_x = img.width()/2;
	int center_y = img.height()/2;
	int center_z = img.depth()/2;

	CImg<float> projected = visuMIP(img,min,max);
	float value = povValue(projected,min,max);
	cout<<"Searching worst direction to start ..."<<endl;

	CImg<float> rot = img;
	rot.rotate(1,0,0,pasX,center_x,center_y,center_z,1,0);
	projected = visuMIP(rot,min,max);
	float valueX1 = povValue(projected,min,max);

	rot = img;
	rot.rotate(1,0,0,-pasX,center_x,center_y,center_z,1,0);
	projected = visuMIP(rot,min,max);
	float valueX2 = povValue(projected,min,max);

	if(valueX1 > valueX2){
		pasX = -pasX;
	}

	rot = img;
	rot.rotate(0,1,0,pasY,center_x,center_y,center_z,1,0);
	projected = visuMIP(rot,min,max);
	float valueY1 = povValue(projected,min,max);

	rot = img;
	rot.rotate(0,1,0,-pasY,center_x,center_y,center_z,1,0);
	projected = visuMIP(rot,min,max);
	float valueY2 = povValue(projected,min,max);

	if(valueY1 > valueY2){
		pasY = -pasY;
	}

	cout<<"...worst first direction found"<<endl;

	rot = img;
	float currentValue = 0;
	float totX = 0;
	float totY = 0;
	int i = 0;
	while(abs(pasX)>pasMin || abs(pasY)>pasMin){
		rot = img;
		if(abs(pasX)>pasMin){
			center_x = img.width()/2;
			center_y = img.height()/2;
			center_z = img.depth()/2;
			totX += pasX;
			rot.rotate(1,0,0,totX,center_x,center_y,center_z,1,0);
			projected = visuMIP(rot,min,max);
			currentValue = povValue(projected,min,max);
			if(currentValue > value){
				cout<<"Changing X direction"<<endl;
				pasX = (pasX*-1)/2;
			}
			value = currentValue;
		}

		if(abs(pasY)>pasMin){
			center_x = img.width()/2;
			center_y = img.height()/2;
			center_z = img.depth()/2;
			totY += pasY;
			rot.rotate(0,1,0,totY,center_x,center_y,center_z,1,0);
			projected = visuMIP(rot,min,max);
			currentValue = povValue(projected,min,max);
			if(currentValue > value){
				cout<<"Changing Y direction"<<endl;
				pasY = (pasY*-1)/ 2;
			}
			value = currentValue;
		}
		i++;
	}
	return rot;
}

int main(int argc, char **argv){

	char nomImgLue[250];
	int min,max,pas,pasMin;

	if(argc != 6){
		cerr<<"Les paramètre attendus sont : nom_img_lue , min et max, le pas de l'algo de recherche et le pas minimum.\n Les valeurs min et max représentent l'intervalle d'intensité de l'objet que l'ont veut visualiser"<<endl;
		return -1;
	}
	sscanf(argv[1],"%s",nomImgLue);
	sscanf(argv[2],"%d",&min);
	sscanf(argv[3],"%d",&max);
	sscanf(argv[4],"%d",&pas);
	sscanf(argv[5],"%d",&pasMin);

	cout<<"Load Image"<<endl;
	CImg<float> img(nomImgLue);
	img.normalize(0,255);

	NB_VOX_IN_INTERVAL = countNbInInterval(img,min,max);
	//Rotation
	int x = 0,y = 0,z = 0;
	int center_x = img.width()/2;
	int center_y = img.height()/2;
	int center_z = img.depth()/2;

	//Getting worst and best rotation according to our min and max values
	//then getting the projection for each
	CImg<float> worst = getWorstRot(img,pas,pas,pasMin,min,max);
	CImg<float> best = getBestRot(img,pas,pas,pasMin,min,max);
	CImg<float> projectedImg = MIP(img/*,min,max*/);
	CImg<float> projectedBest = MIP(best/*,min,max*/);
	CImg<float> projectedWorst = MIP(worst/*,min,max*/);

	cout<<"pdv value : "<<povValue(projectedImg,min,max)<<endl;
	cout<<"Best pdv value : "<<povValue(projectedBest,min,max)<<endl;
	cout<<"Worst pdv value : "<<povValue(projectedWorst,min,max)<<endl;

	projectedImg.display();
	projectedBest.display();
	projectedWorst.display();

	projectedWorst.save("worst.jpg");
	projectedBest.save("best.jpg");
	projectedImg.save("base_img.jpg");

}
