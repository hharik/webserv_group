#ifndef REQUEST_HPP_

#define REQUEST_HPP_

#include "parsing.hpp"

class request {
	const data_serv		*server_data;
	data_header 		*d_header;
	std::string file;
	std::string dir_to_upload;
	long rsize;
	int chunked_size;
	int size;

	public:
	bool end_of_file;
	std::fstream file_obj;

	request( const data_serv *, data_header* );
	~request();
	std::string time_date();
	/* HERE IS THE EMMDDD */
	void	save_chunk_improve(std::string &body);
	void save_binary(std::string &header);
	void read_body(std::string &body);
	void parse(std::string &header);

	/* this will be added in this header 
	(mzhmini lhih o jbthom lhna hada makan) */
	/*check if requested resorce in such location */
	void	parse_the_uri();
	int		update_the_uri( std::string& );
	int		find_required_location( );
	/* check if this location have a redirection */
	int		check_for_redirection();
	/* if this method is allowed in this location */
	int		allowed_methods();
	int		get_requested_resource();
	int		treat_target_resource( std::string, std::string, std::string& );
	int		path_is_exist( std::string& );
	int 	handle_GetAndDelete();
	int 	handle_PostMethod();
	void	default_root( std::string, std::string& );
	void	ProvideToUpload( std::string );
	const std::string	get_extension( const std::string& target );
	/* check if location has upload enabled or not*/

	int	generate_name();


};
#endif
		
		