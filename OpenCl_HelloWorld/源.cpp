
// OpenCl-1.cpp : Defines the entry point for the console application.

//

//#include "stdafx.h" ��Ŀ����c/c++Ԥ����ͷ�ﱾ������



#include <CL/cl.h>

#include <iostream>

#include <fstream>

#include <stdio.h>

#include <stdlib.h>

#include <string>



using namespace std;



cl_int ConvertToString(const char *pFileName, std::string &str);



int main()

{

	cl_int			iStatus = 0;				// ��������״̬

	cl_uint			uiNumPlatforms = 0;				// ƽ̨����

	cl_platform_id	Platform = NULL;				// ѡ���ƽ̨

	size_t			uiSize = 0;				// ƽ̨�汾�����ֽ���	

	cl_int			iErr = 0;				// ���ز���

	char			*pName = NULL;				// ƽ̨�汾��

	cl_uint			uiNumDevices = 0;				// �豸����

	cl_device_id	*pDevices = NULL;				// �豸

	cl_context		Context = NULL;				// �豸����

	cl_command_queue	CommandQueue = NULL;			// �������

	const char		*pFileName = "HelloWorld_Kernel.cl";	// cl�ļ���

	string			strSource = "";				// ���ڴ洢cl�ļ��еĴ���

	const char		*pSource;							// �����ַ���ָ��

	size_t			uiArrSourceSize[] = { 0 };			// �����ַ�������

	cl_program		Program = NULL;				// �������

	const char		*pInput = "gdkknvnqkc";		// �����ַ���

	size_t			uiStrlength = strlen(pInput);	// �����ַ�������

	char			*pOutput = NULL;				// ����ַ���

	cl_mem			memInutBuffer = NULL;				// �����ڴ����

	cl_mem			memOutputBuffer = NULL;				// ����ڴ����

	cl_kernel		Kernel = NULL;				// �ں˶���

	size_t			uiGlobal_Work_Size[1] = { 0 };		// �����趨�ں˷ֲ�	





	//-------------------1. ��ò�ѡ�����ƽ̨-----------------------------

	// ��ѯ���õ�ƽ̨������������״̬

	iStatus = clGetPlatformIDs(0, NULL, &uiNumPlatforms);

	if (CL_SUCCESS != iStatus)

	{

		cout << "Error: Getting platforms error" << endl;

		return 0;

	}





	// ���ƽ̨��ַ

	if (uiNumPlatforms > 0)  // ����п���ƽ̨

	{

		// ����ƽ̨��Ϊƽ̨�����ڴ�ռ�

		cl_platform_id *pPlatforms = (cl_platform_id *)malloc(uiNumPlatforms * sizeof(cl_platform_id));



		// ��ÿ��õ�ƽ̨

		iStatus = clGetPlatformIDs(uiNumPlatforms, pPlatforms, NULL);

		Platform = pPlatforms[0];	// ��õ�һ��ƽ̨�ĵ�ַ

		free(pPlatforms);			// �ͷ�ƽ̨ռ�õ��ڴ�ռ�

	}



	// ���ƽ̨�汾��

	// ���ƽ̨�汾�����ֽ���

	iErr = clGetPlatformInfo(Platform, CL_PLATFORM_VERSION, 0, NULL, &uiSize);



	// �����ֽ���Ϊƽ̨�汾�������ڴ�ռ�

	pName = (char *)alloca(uiSize * sizeof(char));



	// ���ƽ̨�汾����

	iErr = clGetPlatformInfo(Platform, CL_PLATFORM_VERSION, uiSize, pName, NULL);

	cout << pName << endl;







	//--------------2. ��ѯGPU�豸����ѡ������豸------------------------

	// ���GPU�豸����

	iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiNumDevices);

	if (0 == uiNumDevices)	// ���û��GPU�豸

	{

		cout << "No GPU device available." << endl;

		cout << "Choose CPU as default device." << endl;



		// ѡ��CPU��Ϊ�豸������豸��

		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, 0, NULL, &uiNumDevices);



		// Ϊ�豸����ռ�

		pDevices = (cl_device_id *)malloc(uiNumDevices * sizeof(cl_device_id));



		// ���ƽ̨

		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_CPU, uiNumDevices, pDevices, NULL);

	}

	else

	{

		pDevices = (cl_device_id *)malloc(uiNumDevices * sizeof(cl_device_id));



		iStatus = clGetDeviceIDs(Platform, CL_DEVICE_TYPE_GPU, uiNumDevices, pDevices, NULL);

	}





	// -------------------3.�����豸����---------------------------------

	// �����豸����

	Context = clCreateContext(NULL, 1, pDevices, NULL, NULL, NULL);

	if (NULL == Context)

	{

		cout << "Error: Can not create context" << endl;

		return 0;

	}



	// -------------------4.�����������--------------------------------------

	// ������1���豸���������

	CommandQueue = clCreateCommandQueue(Context, pDevices[0], 0, NULL);

	if (NULL == CommandQueue)

	{

		cout << "Error: Can not create CommandQueue" << endl;

		return 0;

	}





	// ----------------------5. �����������------------------------------

	// ��cl�ļ��еĴ���תΪ�ַ���

	iStatus = ConvertToString(pFileName, strSource);



	pSource = strSource.c_str();			// ���strSourceָ��

	uiArrSourceSize[0] = strlen(pSource);	// �ַ�����С



	// �����������

	Program = clCreateProgramWithSource(Context, 1, &pSource, uiArrSourceSize, NULL);

	if (NULL == Program)

	{

		cout << "Error: Can not create program" << endl;

		return 0;

	}





	// -----------------------------6. �������--------------------------------

	// �������

	iStatus = clBuildProgram(Program, 1, pDevices, NULL, NULL, NULL);

	if (CL_SUCCESS != iStatus)	// �������

	{

		cout << "Error: Can not build program" << endl;

		char szBuildLog[16384];

		clGetProgramBuildInfo(Program, *pDevices, CL_PROGRAM_BUILD_LOG, sizeof(szBuildLog), szBuildLog, NULL);



		cout << "Error in Kernel: " << endl << szBuildLog;

		clReleaseProgram(Program);



		return 0;

	}



	//-------------------------7. ��������������ں��ڴ����--------------------------------



	// ���������ڴ����

	memInutBuffer = clCreateBuffer(

		Context,

		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,  // �����ڴ�Ϊֻ���������Դ��������ڴ渴�Ƶ��豸�ڴ�

		(uiStrlength + 1) * sizeof(char),		  // �����ڴ�ռ��С

		(void *)pInput,

		NULL);



	// ��������ڴ����

	memOutputBuffer = clCreateBuffer(

		Context,

		CL_MEM_WRITE_ONLY,					// ����ڴ�ֻ��д

		(uiStrlength + 1) * sizeof(char),	// ����ڴ�ռ��С

		NULL,

		NULL);



	if ((NULL == memInutBuffer) || (NULL == memOutputBuffer))

	{

		cout << "Error creating memory objects" << endl;

		return 0;

	}



	//--------------------------8. �����ں˶���-------------------------------------

	Kernel = clCreateKernel(Program,

		"helloworld",  // cl�ļ��е���ں���

		NULL);

	if (NULL == Kernel)

	{

		cout << "Error: Can not create kernel" << endl;

		return 0;

	}







	//----------------------------9. �����ں˲���----------------------------------

	iStatus = clSetKernelArg(Kernel,

		0,		// ��������

		sizeof(cl_mem),

		(void *)&memInutBuffer);



	iStatus |= clSetKernelArg(Kernel, 1, sizeof(cl_mem), (void *)&memOutputBuffer);



	if (CL_SUCCESS != iStatus)

	{

		cout << "Error setting kernel arguments" << endl;

	}







	// --------------------------10.�����ں�---------------------------------

	uiGlobal_Work_Size[0] = uiStrlength;  // �����ַ�����С



	// �����������ʹ�����豸��ִ�е��ں��Ŷ�

	iStatus = clEnqueueNDRangeKernel(

		CommandQueue,

		Kernel,

		1,

		NULL,

		uiGlobal_Work_Size,  // ȷ���ں����豸�ϵĶ������Ԫ��ķֲ�

		NULL,				 // ȷ���ں����豸�ϵĶ������Ԫ��ķֲ�

		0,

		NULL,

		NULL);





	if (CL_SUCCESS != iStatus)

	{

		cout << "Error: Can not run kernel" << endl;

		return 0;

	}



	// ----------------------------11. �������ȡ�������ڴ�

	pOutput = (char *)malloc(uiStrlength + 1);  // uiStrlength Ϊ �����ַ�������



	iStatus = clEnqueueReadBuffer(

		CommandQueue,		// �������

		memOutputBuffer,	// ����ڴ����

		CL_TRUE,			// �ں˶�ȡ����֮ǰ�ú������᷵��

		0,

		uiStrlength * sizeof(char),

		pOutput,

		0,

		NULL,

		NULL);



	if (CL_SUCCESS != iStatus)

	{

		cout << "Error: Can not reading result buffer" << endl;

		return 0;

	}





	// ---------------------12--���������---------------

	pOutput[uiStrlength] = '\0';

	cout << "Input String:" << endl;

	cout << pInput << endl;

	cout << "Output String:" << endl;

	cout << pOutput << endl;





	// -------------------------------13. �ͷ���Դ--------------------------------

	iStatus = clReleaseKernel(Kernel);

	iStatus = clReleaseProgram(Program);

	iStatus = clReleaseMemObject(memInutBuffer);

	iStatus = clReleaseMemObject(memOutputBuffer);

	iStatus = clReleaseCommandQueue(CommandQueue);

	iStatus = clReleaseContext(Context);



	if (NULL != pOutput)

	{

		free(pOutput);

		pOutput = NULL;

	}



	if (NULL != pDevices)

	{

		free(pDevices);

		pDevices = NULL;

	}



	system("pause");//����ִ��������

	return 0;

}



// ��cl�ļ�����תΪ�ַ���

cl_int ConvertToString(const char *pFileName, std::string &Str)

{

	size_t		uiSize = 0;

	size_t		uiFileSize = 0;

	char		*pStr = NULL;

	std::fstream fFile(pFileName, (std::fstream::in | std::fstream::binary));





	if (fFile.is_open())

	{

		fFile.seekg(0, std::fstream::end);

		uiSize = uiFileSize = (size_t)fFile.tellg();  // ����ļ���С

		fFile.seekg(0, std::fstream::beg);

		pStr = new char[uiSize + 1];



		if (NULL == pStr)

		{

			fFile.close();

			return 0;

		}



		fFile.read(pStr, uiFileSize);				// ��ȡuiFileSize�ֽ�

		fFile.close();

		pStr[uiSize] = '\0';

		Str = pStr;



		delete[] pStr;



		return 0;

	}



	cout << "Error: Failed to open cl file\n:" << pFileName << endl;



	return -1;

}
