//***************************//
//*Template by pulgamecanica*//
//***************************//

#ifndef __ZAPPY_HPP__
# define __ZAPPY_HPP__

#include <iostream>

# define BLACK  "\033[0;30m"
# define RED  "\033[0;31m"
# define GREEN  "\033[0;32m"
# define YELLOW "\033[0;33m"
# define BLUE "\033[0;34m"
# define WHITE  "\033[0;37m"
# define ENDC "\033[0m"


void setnonblocking(int fd);
ssize_t gettimeofday_ms();

#endif
