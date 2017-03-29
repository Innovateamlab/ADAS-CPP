#include "header.hpp"

using namespace std;
using namespace cv;

// Our lamps:
// (These are wiringPi pin numbers)



std::vector<RecognizedShape> shapeDetectorBlue(cv::Mat source, std::vector<std::vector<cv::Point> > contours) 
{
	
	
	// Find shape
	cv::Mat img_cropped;
	std::vector<cv::Point> approx;
	cv::Mat dst = source.clone();
	int thickness = 2;
	cv::Mat imgC;
	
	std::vector<RecognizedShape> recognizedShapes;
	RecognizedShape recognizedShape;
	
	/*RecognizedShape recognizedShape;
	recognizedShape.label = "UNKNOWN";*/
	
	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects 
		if (std::fabs(cv::contourArea(contours[i])) < 1000 || !cv::isContourConvex(approx))
			continue;

		/// RECTANGLE
		if (approx.size() == 4) //&& approx.size() <= 6) 
		{
			// Skip little square
			if (fabs(approx[0].y - approx[1].y) < 200)
			continue; 
			
			// Number of vertices of polygonal curve
			int vtc = approx.size();

			// Get the cosines of all corners
			std::vector<double> cos;
			for (int j = 2; j < vtc+1; j++)
				cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));

			// Sort ascending the cosine values
			std::sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			double mincos = cos.front();
			double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour
			/// SQUARE
			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
			{
				setLabel(dst, "BLUE RECT", cv::boundingRect(contours[i]));
				cv::Rect r = cv::boundingRect(approx);
				cv::Point pt(r.x, r.y);
	
				// Display the rectangle r
				cv::rectangle(dst,pt,pt+cv::Point(r.width,r.height),CV_RGB(255,0,255),thickness);
				cv::Mat img_cropped = source(r);
						  
				// Get x coordinate of square top corner
				int maxx, maxy;
				int minx, miny;
				maxx = r.x + r.width;
				minx = r.x;
				maxy = r.y;
				miny = r.y - r.height;
				int xp = maxx;
				//int xvar, yvar = 0;
				int occurrence = 0;
				
				for (int j=0; j<contours.size(); j++)
				{
					if (contours[i][j].y == maxy)
						{
							occurrence = occurrence + 1;
							xp = contours[i][j].x;
							//cout << "\n\nxvar1 : "<< xp <<"\ncontours[i][j] : "<< contours[i][j] << "\noccurrence : "<< occurrence << endl;
							if (xp < 1.05*minx || xp > 0.95*maxx)
							{
								xp = maxx;
								cout << "Carré déjà cadré ! " << endl;
								break;
							}
							
						}	
						
					/*if (occurrence > 1)
					{
						xp = maxx;
						break;
					} */
						
					/*
					if (contours[i][j].y == miny)
						{
							xvar = contours[i][j].x;
							cout << "\n\nxvar2 : "<< xvar<<"\ncontours[i][j] : "<< contours[i][j] << endl;
							if (xvar < 1.05*minx || xvar > 0.95*maxx)
							{
								xp = maxx;
								cout << "Carré déjà cadré ! " << endl;
								break;
							}	
						}
					
					if (contours[i][j].x == maxx)
						{
							yvar = contours[i][j].y;
							cout << "\n\nyvar3 : "<< yvar<<"\ncontours[i][j] : "<< contours[i][j] << endl;
							if (yvar > 0.95*miny || yvar < 0.05*maxy)
							{
								xp = maxx;
								cout << "Carré déjà cadré ! " << endl;
								break;
							}	
						}	
						
					if (contours[i][j].x == minx)
						{
							yvar = contours[i][j].y;
							cout << "\n\nyvar4 : "<< yvar<<"\ncontours[i][j] : "<< contours[i][j] << endl;
							if (yvar > 0.95*miny || yvar < 0.05*maxy)
							{
								xp = maxx;
								cout << "Carré déjà cadré ! " << endl;
								break;
							}	
						} */
				}
				
				// Rotate
				cv::Mat dst = FeatureRotation(img_cropped,r,xp, approx.size());
				//display
				/* cv::imshow("blue rectangle rotated", dst);
				cv::waitKey(30); */
				
				// CROP THE ROTATED ROAD SIGN
				//0 Define label diag size : R(dst)/R(rotated_crop) = R(rotated_crop)/R(RoadSign); R1/R2 = R2/R3
				int R1, R2, R3 = 1; // R1>R2>R3>0; R3 = (R2 * R2)/R1;  				
				cv::Rect s = resizeToSquare(r);			
				R2 = s.width / sqrt(2);
				R1 = (dst.size().width) / sqrt(2);
				if (R1 == 0)
				R1=R2;
				//1 Define R3 with R1 and R2
				R3 = (R2*R2)/R1;	// R3 is the radius of the road sign square

				//2 Define side of the square Roadsign  with R3
				int C, delta_h, delta_w; // C = side of the road sign square
				C = sqrt(2)*R3;		
				delta_h = (dst.size().height - C); 
				delta_w = (dst.size().width - C);
				
				//3 Create Rect crop and crop
				cv::Point ptR(delta_w/2,delta_h/2);	
				cv::Rect R(ptR,ptR+cv::Point(C,C));
				cv::Mat good = dst(R);
					
				//4 Resize in 100x100
				imgC = SetSizeSquareMat(good,100);
				
				//5 Display image cropped
				/*cv::imshow("blue rectangle",imgC1);
				cv::waitKey(30); */
				
				recognizedShape.matrix = imgC;
				recognizedShape.label = "BLUE_RECT";
				recognizedShape.boundingRect = r;
				
				recognizedShapes.push_back(recognizedShape);
			}	
			//else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27)
				//setLabel(dst, "PENTA", cv::boundingRect(contours[i]));
			//else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45)
				//setLabel(dst, "HEXA", cv::boundingRect(contours[i]));
		}
		else
		{	/// CIRCLE
			
			// Detect and label circles
			double area = cv::contourArea(contours[i]);
			cv::Rect r = cv::boundingRect(contours[i]);
			int radius = r.width / 2;

			if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
			    std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
			{
			// blink led
			//digitalWrite (1, HIGH) ; delay (200) ;
			//digitalWrite (1,  LOW) ; delay (500) ;
			
				setLabel(dst, "BLUE CIR", cv::boundingRect(contours[i]));
				cv::Rect r = cv::boundingRect(approx);
				cv::Point pt(r.x, r.y);
				// Set h and w in the same size, in the size of the highest
				cv::Rect s = resizeToSquare(r);
				
				// Display the rectangle = square
				cv::rectangle(dst,pt,pt+cv::Point(s.width,s.height),CV_RGB(255,0,255),thickness);
				cv::Mat img_cropped = source(s);
				
				//Resize in 100x100
				imgC = SetSizeSquareMat(img_cropped,100);
			/*	cv::imshow("blue circle", imgC2 );
				cv::waitKey(30);*/	
				
				recognizedShape.matrix = imgC;
				recognizedShape.label = "BLUE_CIRC";
				recognizedShape.boundingRect = r;
				
				recognizedShapes.push_back(recognizedShape);
			}
		}
	}
	/*cv::imshow("dst_blue", dst);
	cv::waitKey(30);*/
		
	return recognizedShapes;
}

std::vector<RecognizedShape> shapeDetectorRed(cv::Mat source, std::vector<std::vector<cv::Point> > contours) 
{
	
	// Find shape
	cv::Mat img_cropped;
	std::vector<cv::Point> approx;
	cv::Mat dst = source.clone();
	int thickness = 2;
	cv::Mat imgC;
	
	std::vector<RecognizedShape> recognizedShapes;
	RecognizedShape recognizedShape;
	
	/*RecognizedShape recognizedShape;
	recognizedShape.label = "UNKNOWN";*/
	
	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects 
		if (std::fabs(cv::contourArea(contours[i])) < 1000 || !cv::isContourConvex(approx))
			continue;

		/// TRIANGLE
		if (approx.size() == 3)
		{
			// Skip small triangle
			/*if (fabs(approx[0].y - approx[1].y) < 100)
				continue; 
			*/
			// Set label
			
			cv::Rect r= cv::boundingRect(approx);
			cv::Point pt(r.x, r.y);
			// Set h and w in the same size, in the size of the highest
			cv::Rect s = resizeToSquare(r);
			// blink led
			//digitalWrite (0, HIGH) ; delay (200) ;
			//digitalWrite (0,  LOW) ; delay (500) ;
			
			// Display the rectangle = square
			//cv::rectangle(dst,pt,pt+cv::Point(s.width,s.height),CV_RGB(255,0,255),thickness);
			
#if DEBUG>=2
			setLabel(source, "RED TRI", cv::boundingRect(contours[i]));    // Triangles
			cv::rectangle(source,pt,pt+cv::Point(s.width,s.height),CV_RGB(255,0,255),thickness);
#endif
			cv::Mat img_cropped = source(r);
			
			// Get x coordinate of square top corner
			int maxx, maxy;
			int xvar=0;
			maxx = r.x + r.width;
			maxy = r.y;
			for (int j=0; abs(j)<contours.size(); j++)
			{
				if (contours[i][j].y == maxy)
					{
						xvar = contours[i][j].x;
						//cout << "\n\nxvar : "<< xvar<<"\ncontours[i][j] : "<< contours[i][j] << endl;
						break;
					}		
			}
	
			//Rotate
			cv::Mat dst = FeatureRotation(img_cropped,r ,xvar,approx.size());
			
			// CROP THE ROTATED ROAD SIGN
			//1 Define the angle alpha (in rad) between (center,x_rot) et (center,r.heigth/2)
			int x_rot = (xvar - (int)(s.x + s.width/2)); 
			int pcote = (s.width/2 - fabs(x_rot));		
			//2 define size of a side of the equi triangle
			int C = sqrt(pcote*pcote + s.height*s.height);	
			//3 def angles alpha and beta, between rect and triangle
			float alpha =  acos(((float) s.width) / ((float) C));
		    float beta = (3.1415/6) - alpha;
			//4 Mediatrice
			int M = (s.width)*sin(beta);
			if (M<0)
			M=0;
			//5 Initialize rectangle and points		 
			cv::Point ptR1(0,0); 
			cv::Point ptR2(dst.size().width,  dst.size().height);
			cv::Rect R(0,0,0,0);
			//6 Define Points and rectangle
			if (alpha == 0) //rotation de + de Pi/4 dans sens horaire ou inverse
			{
				ptR1.x = M/2; ptR1.y = 0; 
				ptR2.x = dst.size().width - M/2; ptR2.y=  dst.size().height - M;
			}
			else if (x_rot<=0) //rotation de - de Pi/4 dans sens inverse
			{	
				ptR1.x = 0; ptR1.y =0;
				ptR2.x = dst.size().width - M; ptR2.y = dst.size().height - M;	
			}
			else //rotation de - de Pi/4 dans sens horaire
			{
				ptR1.x = M; ptR1.y = 0; 
				ptR2.x = dst.size().width; ptR2.y = dst.size().height - M;
			}
			
			R.x = ptR1.x;
			R.y = ptR1.y;
			R.width = ptR1.x + ptR2.x;
			R.height = ptR1.y + ptR2.y;
			//cout<< "\nR.w : " << R.width << "\nR.h : "<< R.height << "\nR.x : " << R.x << "\nR.y : " <<R.y << "\ndst.size().width :"<< dst.size().width << "\ndst.size().height :" << dst.size().height <<endl;
			cv::rectangle(dst,ptR1,ptR2,CV_RGB(255,255,0),thickness);
			
			imgC = SetSizeSquareMat(dst,100);
			
			recognizedShape.matrix = imgC;
			recognizedShape.label = "RED_TRI";
			recognizedShape.boundingRect = r;
			
			recognizedShapes.push_back(recognizedShape);
			
			
			/*cv::Mat dst2 = dst.clone();	
			//7 Crop (mais ne fonction pas)
			if ((R.width>0) && (R.height>0) && (R.width<=dst.size().width) && (R.height>=dst.size().height))
				{// Ne fonctionne pas car le crop du crop est plus grand que le crop. Etrange?
					cv::Mat crop= dst2(R);
					
					cv::imshow("\nCROPTTRIEPSDKPN : ", crop);
					cv::waitKey(30); 
				}
				
			// Resize in 100x100
			
			imgC = SetSizeSquareMat(dst,100);*/
			// Displays image cropped
#ifdef DEBUG
			//cv::imshow("red triangle", dst );
			//cv::waitKey(30);
#endif
				

		}
		/// RED CIRCLE
		else
		{
			
			// Detect and label circles
			double area = cv::contourArea(contours[i]);
			cv::Rect r = cv::boundingRect(contours[i]);
			int radius = r.width / 2;

			if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
			    std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
			{
				// blink led
				//digitalWrite (0, HIGH) ; delay (200) ;
				//digitalWrite (0,  LOW) ; delay (500) ;
					
				//setLabel(dst, "RED CIR", cv::boundingRect(contours[i]));
				cv::Rect r = cv::boundingRect(approx);
				cv::Point pt(r.x, r.y);
				
				// Set h and w in the same size, in the size of the highest
				cv::Rect s = resizeToSquare(r);
				
				// Display the rectangle = square
				//cv::rectangle(dst,pt,pt+cv::Point(s.width,s.height),CV_RGB(255,0,255),thickness);
				
#if DEBUG>=2
				setLabel(source, "RED CIRC", cv::boundingRect(contours[i]));  
				cv::rectangle(source,pt,pt+cv::Point(s.width,s.height),CV_RGB(255,0,255),thickness);
#endif
				
				cv::Mat img_cropped = source(s);
				
				//Resize in 100x100
				imgC = SetSizeSquareMat(img_cropped,100);
				/*cv::imshow("red cir", imgC4);
				cv::waitKey(30);*/
				//cout << "\n\nPOint x: "<< s.x << "\nPoint y : "<< s.y << "\nTaille W : "<< s.width << "\nTaille H : " << s.height << endl;
				
				recognizedShape.matrix = imgC;
				recognizedShape.label = "RED_CIRC";
				recognizedShape.boundingRect = r;
				
				recognizedShapes.push_back(recognizedShape);
			}
		}
	}
	//cv::imshow("dst_red", dst);
	//cv::waitKey(30);
	
	return recognizedShapes;
}
