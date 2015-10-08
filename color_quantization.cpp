#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<iostream>
#include <stdlib.h>     // std::string, std::stoi

//function from printing mat details 
void printDim(cv::Mat mat,const char *name){
	std::cout<<name<<" :"<<mat.rows<<"x"<<mat.cols<<"  depth :"<<mat.depth()<<"  channel :"<<mat.channels()<<std::endl;
}

void help(){
	std::cout<<"Usage:"<<std::endl;
	std::cout<<" "<<std::endl;
	std::cout<<" ./color_quantization path_to_image K"<<std::endl;
	std::cout<<" "<<std::endl;
	std::cout<<"where K is number of desired quantization levels."<<std::endl;
}

cv::Mat vectorQuantization(cv::Mat,cv::Mat);
cv::Mat centroidHistogram(cv::Mat,cv::Mat,int);

int main(int argc, char** argv){
	if(argc<3){
		std::cout<<"insufficient arguments"<<std::endl;
		help();
		return -1;
	}
	
	cv::Mat src_img=cv::imread(argv[1]);
	char *str =argv[2];
	if(!src_img.data){
		std::cout<<"no image read!"<<std::endl;
		return -1;
	}
	//optional
	//cv::GaussianBlur(src_img,src_img,cv::Size(7,7),0,0);
	int K=atoi(str);
	cv::Mat labels,centers;
	cv::Mat src_reshaped=src_img.reshape(1,src_img.rows*src_img.cols);
	src_reshaped.convertTo(src_reshaped,CV_32FC1);
	
	kmeans(src_reshaped,K,labels,cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),3, cv::KMEANS_RANDOM_CENTERS,centers);
	centers.convertTo(centers,CV_8UC1);
	labels.convertTo(labels,CV_8UC1);

	cv::Mat quantized=vectorQuantization(labels,centers);
	cv::Mat hist_image=centroidHistogram(labels,centers,K);
	
	quantized=quantized.reshape(3,src_img.rows);
	quantized.convertTo(quantized,CV_8UC3);
	
	cv::imshow("Quantized",quantized);
	cv::imshow("Histogram", hist_image );
	cv::waitKey(0);
}

cv::Mat vectorQuantization(cv::Mat vec,cv::Mat centers){
	// accept only char type matrices
	CV_Assert(vec.depth() != sizeof(uchar));
	
	int rows=vec.rows;
	int cols=vec.cols;
	cv::Mat quantize(rows,centers.cols,centers.type());
	uchar *q=quantize.ptr<uchar>(0);
	uchar *v=vec.ptr<uchar>(0);
	uchar *c=centers.ptr<uchar>(0);
	for(int i=0;i<rows;i++){
	
		q[3*i]=c[3*v[i]];
		q[3*i+1]=c[3*v[i]+1];
		q[3*i+2]=c[3*v[i]+1];
	
	}
	return quantize;
}

cv::Mat  centroidHistogram(cv::Mat vec,cv::Mat centers, int K){
	cv::MatND hist;
	float range[] = { 0, K } ;
	const float* hist_range = { range };
  	bool uniform = true; bool accumulate = false;
	int K_1=K;
	int chan=0;
	double min,max;
	cv::calcHist(& vec,1,&chan,cv::Mat(),hist,1,&K_1,&hist_range, uniform,accumulate);

	//DRAW
	int hist_w = K*50; int hist_h = 150;
	
	int bin_w = cvRound( (double) hist_w/K );
	cv::Mat hist_image( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );

	//normalizing
	cv::minMaxLoc(hist,&min,&max);
	hist=(hist/max)*hist_image.rows;
	cv::Scalar color;
	//replace the labels with the corresponding colors
	for( int i = 1; i <= K; i++ ){
		color[0]=(double)centers.at<uchar>(i-1,0);
		color[1]=(double)centers.at<uchar>(i-1,1);
		color[2]=(double)centers.at<uchar>(i-1,2);
		cv::rectangle(hist_image,cv::Point(bin_w*(i-1),hist_h-cvRound(hist.at<float>(i-1))),cv::Point(bin_w*(i),hist_h),color,-1,8,0);
	}
	return hist_image;

}

