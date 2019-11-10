#
# @file Makefile
# @author m.shebanow
# @date 11/09/2019
# @brief Master makefile
#

docs:
	@doxygen
	@open doxygen/html/index.html

