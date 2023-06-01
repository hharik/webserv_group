#ifndef REQUEST_HPP_

#define REQUEST_HPP_

#include "parsing.hpp"

class request {
	const data_serv		*server_data;
	data_header 		*d_header;
	std::string file;
	long rsize;
	int chunked_size;
	int size;

	public:
	bool end_of_file;
	std::fstream file_obj;

	request( const data_serv *, data_header* );
	~request();
	// data_header getData_header() const {return d_header;}
	std::string time_date();
	std::string copy_until(std::string::iterator first, std::string::iterator last, std::string to_find);
	void	save_chunk_improve(std::string &body);
	/* HERE IS THE EMMDDD */
	void save_chunked(std::string &body);
	void save_binary(std::string &header);

	void parse(std::string &header);

};
#endif
		