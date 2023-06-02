#ifndef RESPONSE_HPP_
#define RESPONSE_HPP_

#include "parsing.hpp"
#include "request.hpp"

class response {
	const data_serv 	*server_data;
	const data_header	*header_data;

	std::string			new_uri;
	std::string			query;
	bool				is_file;

	std::string 		start_line;
	std::string			headers;
	std::string			body;

	std::string			date;
	std::string			server;
public:
	response( const data_serv *, const data_header* );
	int		find_required_location(  );
	void	create_header();
	void	parse_the_uri();
	std::string generateAutoIndex(std::string &directory);
	int		update_the_uri();
};

#endif