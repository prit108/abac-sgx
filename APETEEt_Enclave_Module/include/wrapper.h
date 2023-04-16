#ifndef __WRAPPER_H__
#define __WRAPPER_H__

#include <string>
#include <unordered_map>

typedef std::unordered_map<std::string, std::string> attribute_map;

struct ABAC_TREE_NODE_t {
    int access;
    std::string attrib_name;
    std::unordered_map<std::string, ABAC_TREE_NODE*> childs;
};
typedef struct ABAC_TREE_NODE_t ABAC_TREE_NODE;


struct POLTREE_t {
    ABAC_TREE_NODE* root;
    POLTREE(ABAC_TREE_NODE* r): root(r) {}
    ABAC_TREE_NODE* get_root() {return root;}
};
typedef struct POLTREE_t POLTREE;


struct ABAC_RULE_t {
    std::string rule_name;
    int access;
    int weekday;
    int daytime;
    attribute_map user_attrib_map;
    attribute_map object_attrib_map;
    ABAC_RULE(const std::string& name_, int access_, int weekday_, const attribute_map& ua, const attribute_map& oa): 
    rule_name(name_), access(access_), weekday(weekday_), daytime(-1), user_attrib_map(ua), object_attrib_map(oa){}

    void set_rule_name(std::string name_arg) {
        rule_name = name_arg;
    }
    void set_access(int access_arg) {
        access = access_arg;
    }
    void set_weekday(int weekday_arg) {
        weekday = weekday_arg;
    }
    void set_daytime(int daytime_arg) {
        daytime = daytime_arg;
    }
    void set_user_attribute(std::string attrib, std::string value) {
        user_attrib_map[attrib] = value;
    }
    void set_object_attribute(std::string attrib, std::string value) {
        object_attrib_map[attrib] = value;
    }
    std::string get_user_attribute_value(std::string attrib) {
        return user_attrib_map[attrib];
    }
    std::string get_object_attribute_value(std::string attrib) {
        return object_attrib_map[attrib];
    }
};
typedef struct ABAC_RULE_t ABAC_RULE;


struct ATTRIB_DICT_WRAPPER_t {
    attribute_map attrib_map;
};
typedef struct ATTRIB_DICT_WRAPPER_t ATTRIB_DICT_WRAPPER;

struct RULE_WRAPPER_t {
    std::unordered_map<std::string, ABAC_RULE*> abac_rules;
};
typedef struct RULE_WRAPPER_t RULE_WRAPPER;



#endif // __WRAPPER_H__