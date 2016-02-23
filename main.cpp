#include <CL/cl.hpp>
#include <vector>
#include <iostream>
#include <cstdio>

using namespace std;

int main()
{
	vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	std::vector<cl::Device> totalDevices;

	for (unsigned int j = 0; j < platforms.size(); j++)
	{
		std::vector<cl::Device> devices;
		platforms[j].getDevices(CL_DEVICE_TYPE_ALL, &devices);

		for (unsigned int i = 0; i < devices.size(); i++)
		{
			cout << "  DEVICE ID = " << totalDevices.size() << ", ";

			totalDevices.push_back(devices[i]);
			cout << "  DEVICE_NAME = " << devices[i].getInfo<CL_DEVICE_NAME>() << endl;
		}

	}
	unsigned int deviceID;
	cout << "Select device: ";
	cin >> deviceID;


	if (deviceID >= totalDevices.size())
	{
		printf("Invalid selection\n");
		return 1;
	}

	cl_int err;
	cl::Device device = totalDevices[deviceID];
	cl::Context context(device);
	cl::CommandQueue queue(context, device);

	cl::ImageFormat format;
	format.image_channel_order = CL_R;
	format.image_channel_data_type = CL_UNSIGNED_INT8;

	size_t width = 8;
	size_t height= 4;
	size_t arraySize = 1;

	cl::Image2DArray image(context, CL_MEM_READ_WRITE, format, arraySize, width, height, 0, 0);

	size_t dim[3];
	image.getImageInfo(CL_IMAGE_WIDTH, &dim[0]);
	image.getImageInfo(CL_IMAGE_HEIGHT, &dim[1]);
	image.getImageInfo(CL_IMAGE_ARRAY_SIZE, &dim[2]);

	printf("Host: %ld %ld %ld\n", dim[0], dim[1], dim[2]);

	const char src[] = "__kernel void imageinfo(__write_only image2d_array_t im){ \
						printf(\"Kernel: %d %d %d\\n\", get_image_width(im), get_image_height(im), get_image_array_size(im));\
						}";
	cl::Program program(context, src);
	err = program.build();

	cl::Kernel kernel(program, "imageinfo");
	err = kernel.setArg(0, image);

	err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1,1,1),  cl::NDRange(1,1,1)  );
	queue.finish();
}
