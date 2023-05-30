#ifndef REQUEST_HPP_

#define REQUEST_HPP_

#include "parsing.hpp"

struct data_header {
	std::string Content_type;
	std::string Host;
	int Content_Length;
	std::string transfer_encoding;
	std::string method;
	std::string uri;
	std::string http_version;

	std::string boundary;
	int res_status;
	data_header() : Content_Length(-2), res_status(-2) {}
	void print() {
		std::cout << " h" << Content_type <<  " " << Host << " " << Content_Length << " " << transfer_encoding << " " << method << std::endl;
	}
	bool empty() {
		if (Host.empty() == true && transfer_encoding.empty() == true && Content_type.empty() == true)
			return true;
		return false;
	}
};

class request {

	data_header d_header;
	std::string file;
	long rsize;
	int chunked_size;
	int size;


	public:
	bool end_of_file;
	std::fstream file_obj;

	request();
	~request();
	data_header getData_header() const {return d_header;}
	std::string time_date();
	std::string copy_until(std::string::iterator first, std::string::iterator last, std::string to_find);
	void	save_chunk_improve(std::string &body);
	/* HERE IS THE EMMDDD */
	void save_chunked(std::string &body);
	void save_binary(std::string &header);

	void parse(std::string &header);

};
#endif
		