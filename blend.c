#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;
typedef unsigned char BYTE;

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType; //specifies the file type
    DWORD bfSize; //specifies the size in bytes of the bitmap file
    WORD bfReserved1; //reserved; must be 0
    WORD bfReserved2; //reserved; must be 0
    DWORD bfOffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits

}   tagBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize; //specifies the number of bytes required by the struct
    LONG biWidth; //specifies width in pixels
    LONG biHeight; //species height in pixels
    WORD biPlanes; //specifies the number of color planes, must be 1
    WORD biBitCount; //specifies the number of bit per pixel
    DWORD biCompression;//spcifies the type of compression
    DWORD biSizeImage; //size of image in bytes
    LONG biXPelsPerMeter; //number of pixels per meter in x axis
    LONG biYPelsPerMeter; //number of pixels per meter in y axis
    DWORD biClrUsed; //number of colors used by th ebitmap
    DWORD biClrImportant; //number of colors that are important

}   tagBITMAPINFOHEADER;

BYTE getColorFromPixel(BYTE * imagedata, int x, int y, int bplo, int colorCode){
                return *(imagedata + x*3 + bplo*y + colorCode);
            } 

void getColor(unsigned char *imagedata, unsigned char *imagedata2, unsigned char *imagedata3, LONG height, 
            LONG width, double ratio){
    
    BYTE colorValue1, colorValue2, colorTarget;
    int bplo = height * width * 3;
    
    if (bplo % 4 != 0){
        bplo = bplo + 4 - bplo % 4; 
    } 
    
    for (int i = 0; i < bplo; i++){
        
        colorValue1 = *(imagedata + i); 
        colorValue2 = *(imagedata2 + i);

        colorTarget = (colorValue1 * ratio) + (colorValue2 * (1-ratio));
        *(imagedata3 + i) = colorTarget; 
    }
}

void getColorDiffSize(BYTE *imagedata, BYTE *imagedata2, BYTE *imagedata3, LONG height1, 
            LONG width1, LONG height2, LONG width2, double ratio){
    
    int i,j,indexAx,indexAy,indexBx,indexBy;
    double xs,ys,tx,ty,txA,tyA; 
    LONG targetWidth, targetHeight, targetSize, smallerWidth, smallerHeight;
    BYTE *smallerImageData, *biggerImageData;

    if (width1 > width2){
        targetWidth = width1;
        targetHeight = height1;
        smallerWidth = width2;
        smallerHeight = height2;
        //targetSize = width1 * height1 * 3;
        smallerImageData = imagedata2;
        biggerImageData = imagedata;
    }

    else if (width1 < width2){
        targetWidth = width2;
        targetHeight = height2;
        smallerWidth = width1;
        smallerHeight = height1;
        //targetSize = width2 * height2 * 3; 
        smallerImageData = imagedata; 
        biggerImageData = imagedata2;
    }
        

    int largeBplo = targetWidth * 3;
    
    if (largeBplo % 4 != 0){
        largeBplo = largeBplo + 4 - largeBplo % 4; 
    }

    int smallBplo = smallerWidth * 3;
    
    if (smallBplo % 4 != 0){
        smallBplo = smallBplo + 4 - smallBplo % 4; 
    }  


    for (i = 0; i < targetHeight; i++){
        for (j = 0; j < targetWidth; j++){
            
            xs = j * ((double)smallerWidth/targetWidth);
            ys = i * ((double)smallerHeight/targetHeight);
            indexAx = (int)xs;
            indexAy = (int)ys;
            indexBx = indexAx + 1;
            indexBy = indexAy + 1;
            tx = xs - indexAx;
            ty = ys - indexAy;
            txA = 1 - tx;
            tyA = 1 - ty;

            BYTE BlueTopLeft = getColorFromPixel(smallerImageData,indexAx,indexAy,smallBplo,0);
            BYTE GreenTopLeft = getColorFromPixel(smallerImageData,indexAx,indexAy,smallBplo,1);
            BYTE RedTopLeft = getColorFromPixel(smallerImageData,indexAx,indexAy,smallBplo,2);

            BYTE BlueTopRight = getColorFromPixel(smallerImageData,indexBx,indexAy,smallBplo,0);
            BYTE GreenTopRight = getColorFromPixel(smallerImageData,indexBx,indexAy,smallBplo,1);
            BYTE RedTopRight = getColorFromPixel(smallerImageData,indexBx,indexAy,smallBplo,2);

            BYTE BlueBottomLeft = getColorFromPixel(smallerImageData,indexAx,indexBy,smallBplo,0);
            BYTE GreenBottomLeft = getColorFromPixel(smallerImageData,indexAx,indexBy,smallBplo,1);
            BYTE RedBottomLeft = getColorFromPixel(smallerImageData,indexAx,indexBy,smallBplo,2);

            BYTE BlueBottomRight = getColorFromPixel(smallerImageData,indexBx,indexBy,smallBplo,0);
            BYTE GreenBottomRight = getColorFromPixel(smallerImageData,indexBx,indexBy,smallBplo,1);
            BYTE RedBottomRight = getColorFromPixel(smallerImageData,indexBx,indexBy,smallBplo,2);

            BYTE TargetBlueLeft = BlueTopLeft*tyA + BlueBottomLeft*ty;
            BYTE TargetBlueRight = BlueTopRight*tyA + BlueBottomRight*ty;
            BYTE TargetBlue = TargetBlueLeft *txA + TargetBlueRight*tx;

            BYTE TargetGreenLeft = GreenTopLeft*tyA + GreenBottomLeft*ty;
            BYTE TargetGreenRight = GreenTopRight*tyA + GreenBottomRight*ty;
            BYTE TargetGreen = TargetGreenLeft *txA + TargetGreenRight*tx;

            BYTE TargetRedLeft = RedTopLeft*tyA + RedBottomLeft*ty;
            BYTE TargetRedRight = RedTopRight*tyA + RedBottomRight*ty;
            BYTE TargetRed = TargetRedLeft *txA + TargetRedRight*tx;
        

            *(imagedata3 + j*3 + largeBplo*i) = TargetBlue*(1-ratio) + (*(biggerImageData + j*3 + largeBplo*i))*ratio;
            *(imagedata3 + j*3 + largeBplo*i + 1) = TargetGreen*(1-ratio) + (*(biggerImageData + j*3 + largeBplo*i + 1))*ratio;
            *(imagedata3 + j*3 + largeBplo*i + 2) = TargetRed*(1-ratio) + (*(biggerImageData + j*3 + largeBplo*i + 2))*ratio;

        }

    }
        
    
}


int main(int argc, char *argv[]){
    
    char *pointer;
    int testratio = strtol(argv[3], &pointer, 10);
    

    if (argc != 5){
        printf("You have entered the wrong number of commands.\n"
                "Please use the following format to run your program:\n"
                "[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n\n");
        
        return -1;
    }

    else if (argv[1] == 0){
        printf("Your first input file was not found."
                "Please use the following format to run your program:\n"
                "[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n\n");
        return -1;
    }

    else if (argv[2] == 0){
        printf("Your first input file was not found."
                "Please use the following format to run your program:\n"
                "[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n\n");

        return -1;

    }

    else if (!strstr(argv[1], ".bmp") || !strstr(argv[2], ".bmp")){
        printf("You have entered the wrong input files.\n"
                "Please use the following format to run your program:\n"
                "[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n"
                "Make sure to use the \".bmp\" extension for your input image files.\n\n");

        return -1;        
    }

    else if (!(0 < testratio < 1)){
        printf("You have entered a wrong ratio number.\n"
                "Please use the following format to run your program:\n"
                "[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n"
                "Make sure to select a ration number between 0 and 1\n\n");
        return -1;        
    }

    else if (!strstr(argv[4], ".bmp")){
        printf("You have entered the wrong output file.\n"
                "Please use the following format to run your program:\n"
                "[programname] [imagefile1] [imagefile2] [ratio] [outputfile]\n"
                "Make sure to use the \".bmp\" extension for your output image file.\n\n");
        return -1;        
    }

    char *ptr;
    double ratio = strtod(argv[3], &ptr);

    tagBITMAPFILEHEADER fh1;
    tagBITMAPINFOHEADER fih1; 

    FILE *input1 = fopen(argv[1], "rb");

    fread(&fh1.bfType,sizeof(WORD),1,input1);
    fread(&fh1.bfSize,sizeof(DWORD),1,input1);
    fread(&fh1.bfReserved1,sizeof(WORD),1,input1);
    fread(&fh1.bfReserved2,sizeof(WORD),1,input1);
    fread(&fh1.bfOffBits,sizeof(DWORD),1,input1);

    fread(&fih1,sizeof(fih1),1,input1);

    BYTE *dataInput1 = (BYTE*)malloc(fih1.biHeight*fih1.biWidth*3);
    fread(dataInput1,fih1.biSizeImage,1,input1);

    fclose(input1);

    tagBITMAPFILEHEADER fh2;
    tagBITMAPINFOHEADER fih2;

    FILE *input2 = fopen(argv[2], "rb");

    fread(&fh2.bfType,sizeof(WORD),1,input2);
    fread(&fh2.bfSize,sizeof(DWORD),1,input2);
    fread(&fh2.bfReserved1,sizeof(WORD),1,input2);
    fread(&fh2.bfReserved2,sizeof(WORD),1,input2);
    fread(&fh2.bfOffBits,sizeof(DWORD),1,input2);

    fread(&fih2,sizeof(fih2),1,input2);

    BYTE *dataInput2 = (BYTE*)malloc(fih2.biHeight*fih2.biWidth*3);
    fread(dataInput2,fih2.biSizeImage,1,input2);

    fclose(input2);

    BYTE *dataOutput;
    tagBITMAPFILEHEADER fh3;
    tagBITMAPINFOHEADER fih3 ;

    if (fih1.biSizeImage == fih2.biSizeImage){
        
        dataOutput = (BYTE*)malloc(fih1.biHeight*fih1.biWidth*3);
        getColor(dataInput1,dataInput2,dataOutput,fih1.biHeight,fih1.biWidth,ratio);
        fh3 = fh1;
        fih3 = fih1;
    }
    
    else if (fih1.biSizeImage != fih2.biSizeImage){
        
        if  (fih1.biWidth > fih2.biWidth){
            dataOutput= (BYTE*)malloc(fih1.biHeight*fih1.biWidth*3);
            fh3 = fh1; 
            fih3 = fih1;
            getColorDiffSize(dataInput1,dataInput2,dataOutput,fih1.biHeight,fih1.biWidth,fih2.biHeight,fih2.biWidth,ratio); 
        }

        else if  (fih1.biWidth < fih2.biWidth){
            fh3 = fh2; 
            fih3 = fih2;
            dataOutput= (BYTE*)malloc(fih2.biHeight*fih2.biWidth*3);
            getColorDiffSize(dataInput2,dataInput1,dataOutput,fih2.biHeight,fih2.biWidth,fih1.biHeight,fih1.biWidth,ratio); 
        }

           
    }

    

    FILE *output = fopen(argv[4], "wb");

    fwrite(&fh3.bfType,sizeof(WORD),1,output);
    fwrite(&fh3.bfSize,sizeof(DWORD),1,output);
    fwrite(&fh3.bfReserved1,sizeof(WORD),1,output);
    fwrite(&fh3.bfReserved2,sizeof(WORD),1,output);
    fwrite(&fh3.bfOffBits,sizeof(DWORD),1,output);

    fwrite(&fih3,sizeof(fih3),1,output);
    fwrite(dataOutput,fih3.biSizeImage,1,output);
    
    fclose(output);

    free(dataInput1);
    free(dataInput2);
    free(dataOutput);

    
    return 0;
}