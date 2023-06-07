	#include <fcntl.h>
	#include <iostream>
	#include <unistd.h>

int main()

{

	char *env[3];
	env[0] = (char *)std::string("/Users/hharik/Desktop/latest_push/cgi-scripts/php-cgi").c_str();
	env[1] = (char *)std::string("/Users/hharik/Desktop/latest_push/root/her.php").c_str();
	env[2] = NULL;
	int cgifd[2];
	cgifd[1] = open("/tmp/test.txt", O_CREAT | O_RDWR, 0777); 
	int child = fork();
	if (child == -1)
	{
		close(cgifd[1]);
		close(cgifd[0]);
		std::cout << "close file failed" << std::endl;
		// exit(1); 
	}
	// for (size_t i = 0; i < 3; i++)
	std::cout << env[0] << std::endl;
	std::cout << env[1] << std::endl;
	std::cout << env[2] << std::endl;

	// if (child == 0)
	// {
	// 	dup2(cgifd[1], 1);
	// 	close(cgifd[1]);
	// 	close(cgifd[0]);
	// 	execve(env[0], env, NULL);
	// 	perror("execve");
	// }
	// else {
	// 	wait(NULL);

	// }

}