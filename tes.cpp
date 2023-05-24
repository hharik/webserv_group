#include <iostream>
#include <string>

// Parse a multipart/form-data request and extract file data
void parseMultipartFormData(const std::string& request) {
    std::string delimiter = "\r\n--"; // Delimiter for each part

    std::size_t pos = request.find(delimiter);
    std::size_t startPos = 0;

    // Skip the initial boundary
    if (pos != std::string::npos) {
        startPos = pos + delimiter.length();
    }

    while (pos != std::string::npos) {
        std::size_t endPos = request.find(delimiter, startPos);

        if (endPos != std::string::npos) {
            std::string part = request.substr(startPos, endPos - startPos);

            // Skip empty parts
            if (!part.empty()) {
                std::size_t headersEnd = part.find("\r\n\r\n") + 4;

                if (headersEnd != std::string::npos) {
                    std::string headers = part.substr(0, headersEnd);
                    std::string content = part.substr(headersEnd);

                    // Process the headers to extract metadata like the file name
                    // You can extract other relevant metadata based on your requirements

                    // Process the content as the file data
                    // You can save the file or perform further operations with the data

                    // Example: Print the part's headers and content
                    std::cout << "Part headers: " << headers << std::endl;
                    std::cout << "Part content: " << content << std::endl;
                    std::cout << "----------------------------------------" << std::endl;
                }
            }

            startPos = endPos + delimiter.length();
        }

        pos = endPos;;s
    }
}

int main() {
    // Example multipart/form-data request string
    std::string request = "--boundary\r\n"
                          "Content-Disposition: form-data; name=\"file1\"; filename=\"example1.txt\"\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n"
                          "File 1 data\r\n"
                          "--boundary\r\n"
                          "Content-Disposition: form-data; name=\"file2\"; filename=\"example2.txt\"\r\n"
                          "Content-Type: text/plain\r\n"
                          "\r\n"
                          "File 2 data\r\n"
                          "--boundary--\r\n";

    // Parse the multipart/form-data request
    parseMultipartFormData(request);

    return 0;
}


"\r\n84f25\r\n"

std::string request =   "POST /upload HTTP/1.1\r\n"
                          "Host: example.com\r\n"
                          "Transfer-Encoding: chunked\r\n"
                         
						  "\r\n"
                          "4\r\n"

                          "Wiki\r\n"
                          "5\r\n"
                          "pedia\r\n"
                          "E\r\n"
                          " in\r\n\r\nchunks.\r\n"

hexa 946 

// chunked size 2374  
// 2381 size without \r\n added
// after deleting first hexa size is 2378 
// still has the \r\n is the first + 2 // changed it to + 4 


chunked  2374 
body size \r\n 2376 \r\n6354\r\n