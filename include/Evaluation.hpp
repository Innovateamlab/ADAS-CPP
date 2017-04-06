#ifndef EVALUATION_H_INCLUDED
#define EVALUATION_H_INCLUDED

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string>

#define threshold 100

typedef struct Annotation
{
	std::string filename;
	int nbObjects;
	std::vector<cv::Rect> boundingRects;
} Annotation;

typedef struct Result
{
	int truePositives, falsePositives;
	int trueNegatives, falseNegatives;
	
	Result operator+(const Result& r) const
	{
		Result result;
		result.truePositives = truePositives + r.truePositives;
		result.falsePositives = falsePositives + r.falsePositives;
		result.trueNegatives = trueNegatives + r.trueNegatives;
		result.falseNegatives = falseNegatives + r.falseNegatives;
		return result;
	}
	
	
} Result;

Result initResult()
{
	Result result;
	result.truePositives = 0;
	result.falsePositives = 0;
	result.trueNegatives = 0;
	result.falseNegatives = 0;
	return result;
}

double rectangleDistance(cv::Rect rect1, cv::Rect rect2)
{
	double norm = sqrt((rect2.x-rect1.x)*(rect2.x-rect1.x) + (rect2.y-rect1.y)*(rect2.y-rect1.y));
	
	double diffW = abs(rect2.width - rect1.width);
	double diffH = abs(rect2.height - rect1.height);
	
	return norm + 0.5*(diffW + diffH);
}

Result getDectectionResult(std::vector<cv::Rect> trueRects, std::vector<cv::Rect> detectionRects)
{
	Result result = initResult();
	std::vector<int> associations;
	double minScore, currentScore, bestRect;
	
	if(detectionRects.size() == 0 && trueRects.size() == 0)
	{
		result.trueNegatives +=1;
		return result;
	}
	
	for(int i=0;i<trueRects.size();i++)
	{
		minScore = 9999;
		for(int j=0;j<detectionRects.size();j++)
		{
			currentScore = rectangleDistance(trueRects[i], detectionRects[j]);
			std::cout << "Score : " << currentScore << "\n";
			if(currentScore < minScore)
			{
				minScore = currentScore;
				bestRect = j;
			}
		}
		
		if(minScore <= threshold)
		{
			//associations.push_back(bestRect);
			result.truePositives +=1;
		}
		else
		{
			//associations[i] = -1;
			result.falseNegatives += 1;
		}
	}
	
	if(detectionRects.size() > trueRects.size())
	{
		result.falsePositives += detectionRects.size() - trueRects.size();
	}
	
	return result;
}

//./images_1/imgGlobal_1.jpg 2 391 230 193 198 391 230 193 198
Annotation initAnnotation(std::string line)
{
	Annotation annotation;
	int tab[4];	
	std::stringstream stream(line);

	stream >> annotation.filename;
	stream >> annotation.nbObjects;

	for(int i=0;i<annotation.nbObjects;i++)
	{	
		int k = 0;	
		while((stream >> tab[k]) && (k < 3))
		{
			k++;
		}
		annotation.boundingRects.push_back(cv::Rect(tab[0], tab[1], tab[2], tab[3]));
	}
	
	return annotation;	
}

std::vector<Annotation> loadAnnotations(std::string annotationFile)
{
	std::vector<Annotation> annotations;
	Annotation annotation;

	std::ifstream file(annotationFile.c_str(), std::ios::in);
	if(!file.is_open()){ std::cout << "Can't open annotation file\n";} 		

	std::string line = "";
	
	while(getline(file, line))
	{
		annotation = initAnnotation(line);
		annotations.push_back(annotation);
	}
	
	file.close();
	
	return annotations;
}

std::string printAnnotation(Annotation annotation)
{
	std::stringstream s;
	
	s << annotation.filename << " ";
	s << annotation.nbObjects << " ";
	
	for(int i=0;i<annotation.boundingRects.size();i++)
	{
		s <<  annotation.boundingRects[i].x;
		s << " ";
		s <<  annotation.boundingRects[i].y;
		s << " ";
		s <<  annotation.boundingRects[i].width;
		s << " ";
		s <<  annotation.boundingRects[i].height;
		s << " ";
	}
	
	return s.str();
}

std::string printResult(Result result)
{
	std::stringstream s;
	
	s << "True positives : " << result.truePositives << "\n";
	s << "False positives : " << result.falsePositives << "\n";
	s << "True negatives : " << result.trueNegatives << "\n";
	s << "False negatives : " << result.falseNegatives << "\n";
	
	
	return s.str();
}









#endif
