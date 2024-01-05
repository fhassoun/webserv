#include "Webserv.hpp"


int Webserv::getMethod() {
    logging("GET request", DEBUG);

    //std::cout << "PATH: " << http_request->path << std::endl;
    char *tmp = string_to_chararray(http_request->path);
    //std::cout << "PATH IN REQUEST: " << tmp << std::endl;

     if (http_request->path.find("download.php") != std::string::npos && access("./over42/download.php", F_OK) == 0) {
         std::cout << "./over42/download.php exists" << std::endl;
         if (access("./over42/download.php", X_OK) == 0) {
             int pipefd[2];
             pipe(pipefd);
             if (fork() == 0) {
                 close(pipefd[0]);
                 dup2(pipefd[1], STDOUT_FILENO);

                 size_t filenamePTR = http_request->path.find("file=");
                std::string filename = http_request->path.substr(filenamePTR + 5);
                 std::string fullName = "file=" + filename;

                 std::string tmp2 = "/over42/download.php";
                 delete[] tmp;
                 tmp = string_to_chararray(tmp2);
                 http_response->path = tmp2;
                 char *const args[] = {tmp, strdup(fullName.c_str()), NULL};
                 //std::cout << "tmp: " << tmp << " args: " << args[0] << std::endl;
                 execve(tmp, args, env);
             } else {
                 close(pipefd[1]);
                 std::vector<char> buffer(1024);
                 std::vector<char> content;

                 ssize_t bytesRead;
                 while ((bytesRead = read(pipefd[0], buffer.data(), buffer.size())) > 0) {
                     content.insert(content.end(), buffer.begin(), buffer.begin() + bytesRead);
                 }
                 std::string bytedtoStr(content.begin(), content.end());
                 http_response->path = bytedtoStr;
                 //std::cout << "scripted http_request->path: " << http_request->path << std::endl;
                 waitpid(-1, NULL, WUNTRACED);

             }
         }
         return 777;
     }

    if (access(tmp, F_OK) == 0) {

        struct stat path_stat;
        stat(tmp, &path_stat);
        bool is_directory = S_ISDIR(path_stat.st_mode);

        if (ft_strcmp(tmp, "./cgi-bin/index.py") == 0 && access("./cgi-bin/index.py", F_OK) == 0) {
            std::cout << "cgi-bin/index.py exists" << std::endl;
            if (access("./cgi-bin/index.py", X_OK) == 0) {

                std::cout << "cgi-bin/index.py exists" << std::endl;
                int pipefd[2];
                pipe(pipefd);
                if (fork() == 0) {
                    close(pipefd[0]);
                    dup2(pipefd[1], STDOUT_FILENO);
                    std::string tmp2 = "/cgi-bin/index.py";
                    delete[] tmp;
                    tmp = string_to_chararray(tmp2);
                    http_response->path = tmp2;
                    char *const args[] = {tmp, NULL};
                    //std::cout << "tmp: " << tmp << " args: " << args[0] << std::endl;
                    execve(tmp, args, env);
                } else {
                    close(pipefd[1]);
                    char buffer[1024];
                    std::string scriptOutput;
                    ssize_t bytesRead;
                    while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
                        buffer[bytesRead] = '\0';
                        scriptOutput += buffer;
                    }
                    // std::cout << "scriptOutput: " << scriptOutput << std::endl;
                    http_response->path = scriptOutput;
                    // std::cout << "scripted http_request.path: " << http_request.path << std::endl;
                    waitpid(-1, NULL, WUNTRACED);

                }
            }
        }
        else if (ft_strcmp(tmp, "./over42/get_files.php") == 0 && access("./over42/get_files.php", F_OK) == 0) {
            std::cout << "/over42/get_files.php exists" << std::endl;
            delete[] tmp;
            return (getCgiFilesList());
        }

        else if (!is_directory)
        {

            if (access(tmp, R_OK) == 0)
            {
                std::cout << "file exists" << std::endl;
                http_response->path = tmp;
            }
            else
            {
                std::cout << "file doesn't exist" << std::endl;
                delete[] tmp;
                return 403;
            }

        }
        else if (is_directory)
        {
            std::string tmp2 = "." + http_request->path;
            http_response->path = autoindex(tmp2); //AUTOINDEX HERE
            // std::cout << "autoindex http_request.path: " << http_request.path << std::endl;
        }
        delete[] tmp;
        //std::cout << "PATH IN REQUEST END: " << http_response->path << std::endl;
        return 200;

    }
    else
    {
        std::cout << "file doesn't exist" << std::endl;
    }
    delete[] tmp;
    return 404;
}

int Webserv::getCgiFilesList() {
    char *tmp = string_to_chararray(http_request->path);
    if (access("./over42/get_files.php", X_OK) == 0) {
        int pipefd[2];
        pipe(pipefd);
        if (fork() == 0) {
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
            std::string tmp2 = "/over42/get_files.php";
            delete[] tmp;
            tmp = string_to_chararray(tmp2);
            http_response->path = tmp2;
            char *const args[] = {tmp, NULL};
            //std::cout << "tmp: " << tmp << " args: " << args[0] << std::endl;
            execve(tmp, args, env);
        } else {
            close(pipefd[1]);
            char buffer[1024];
            std::string scriptOutput;
            ssize_t bytesRead;
            while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[bytesRead] = '\0';
                scriptOutput += buffer;
            }
            //std::cout << "scriptOutput: " << scriptOutput << std::endl;
            http_response->path = scriptOutput;
            //std::cout << "scripted http_request->path: " << http_request->path << std::endl;
            waitpid(-1, NULL, WUNTRACED);

        }
        delete[] tmp;
        return 999;
    } else {
        delete[] tmp;
        return 404;
    }
}