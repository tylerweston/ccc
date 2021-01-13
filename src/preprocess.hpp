#include <string>

enum class preprocess_state
{
	normal = 0,
	normal_seen_slash,
	in_block_comment,
	in_block_comment_seen_star
};

class preprocess
{
public:
	int preprocess_file(const std::string, const std::string);	
};

