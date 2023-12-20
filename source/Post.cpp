#include "Webserv.hpp"

void Webserv::postMethod(int i) {
    int clientFD = poll_fd[i].fd;
    if (http_requests.count(clientFD) == 0) {
        http_requests[clientFD] = http_request;

        std::cout << "START CONTENT" << std::endl;
        //std::cout << "CONTENT : " << http_request.content << std::endl;

    }
    const char *ContLen = http_request.headers["Content-Length"].c_str();
    size_t content_length = static_cast<size_t>(std::atoi(ContLen));
    if (http_request.content.size() == content_length) {
        http_requests.erase(clientFD);
        std::cout << "FINISH CONTENT" << std::endl;
        out_response[clientFD] = post_getdata();
        //file formed to temporary location
        //business logic decide to safe it to permanent folder while saving check if all requirements are meet
    } else if (http_request.content.size() > content_length || http_request.content.size() > content_length) {
        std::cout << "SUPPOSED content_length : " << content_length << std::endl;
        std::cout << "content size " << http_request.content.size() << std::endl;
        http_requests.erase(clientFD);
        out_response[clientFD] = "HTTP/1.1 400 Bad Request\r\n";

        std::cout << "CORRUPTED CONTENT" << std::endl;
    } else {
        std::cout << "PARTIAL CONTENT " << http_request.content.size() << " of " << content_length << std::endl;
        out_response[clientFD] = "HTTP/1.1 200 OK\r\n";

    }
}

std::string Webserv::post_getdata() {
    if (http_request.path == "/cgi-bin/index.py" || http_request.path == "/submit/") {
        return formPostResponse();
    } else if (http_request.path == "/over42/upload") {
        postContentProcess();
    }
    return formPostResponse();
}

void Webserv::setMetaData() {
    std::istringstream lineStream(std::string(http_request.contentHead.begin(), http_request.contentHead.end()));
    std::string line;
    while (std::getline(lineStream, line) && !line.empty()) {
        std::string fileNamePrefix = "filename=\"";
        std::string fileNameSuffix = "\"";
        size_t fileNameStartOffset = line.find(fileNamePrefix);
        if (fileNameStartOffset != std::string::npos) {
            fileNameStartOffset += fileNamePrefix.length();
            size_t fileNameEndOffset = line.find(fileNameSuffix, fileNameStartOffset);
            if (fileNameEndOffset != std::string::npos) {
                size_t fileNameLength = fileNameEndOffset - fileNameStartOffset;
                metaD.filename = line.substr(fileNameStartOffset, fileNameLength);
            }
        }
        size_t typePtr = line.find("Content-Type: ");
        if (typePtr != std::string::npos) {
            typePtr += 14;
            size_t typeLen = line.length() - typePtr;
            metaD.content_type = line.substr(typePtr, typeLen);
        }
    }
    metaD.location = "download/";
    metaD.fullPath = metaD.location + metaD.filename;
}

void Webserv::postContentProcess() {
    if (http_request.content.empty())
        return;
    size_t delimIndex = 0;
    if (!canSeparate(http_request.content, delimIndex))
        return;
    http_request.contentHead.assign(http_request.content.begin(), http_request.content.begin() + delimIndex + 4);
    setMetaData();
    //erase header from content and last boundary
    http_request.content.erase(http_request.content.begin(), http_request.content.begin() + delimIndex + 4);
    //+4 for "--" in the beginning and end + 1013 after the content
    http_request.content.erase(http_request.content.end() - (http_request.boundary.size() + 6), http_request.content.end());

    std::ofstream outputFile;
    outputFile.open(metaD.fullPath.c_str(), std::ios::binary);
    if (outputFile.is_open()) {

        outputFile.write(reinterpret_cast<const char *>(&http_request.content[0]), http_request.content.size());

        // Close the file when done
        outputFile.close();
        std::cout << "File created successfully: " << metaD.fullPath << std::endl;
    } else {
        std::cerr << "Failed to create file: " << metaD.fullPath << std::endl;
    }

}

// Helper function to convert integer to string
static std::string intToString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string Webserv::formPostResponse() {
    std::string response;
    http_response.status_code = 200;

    // Set the content type to HTML
    http_response.headers["Content-Type"] = "text/html";

    // Build the response body
    response += "<html><body>";
    response += "Your data received by us";
    //response += http_request.content; //UNCPMENT
    response += "</body></html>";

    // Set the content length in the headers
    std::ostringstream oss;
    oss << response.size();
    http_response.headers["Content-Length"] = oss.str();
    std::string full_response = "HTTP/1.1 " + intToString(http_response.status_code) + " OK\r\n";

    // Iterate over headers
    for (std::map<std::string, std::string>::const_iterator it = http_response.headers.begin();
         it != http_response.headers.end(); ++it) {
        full_response += it->first + ": " + it->second + "\r\n";
    }

    full_response += "\r\n" + response;
    return full_response;
}


