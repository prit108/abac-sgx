#include "enclave_t.h"
#include "sgx_tseal.h"
#include "string.h"

#include "enclave.h"
#include "wrapper.h"
#include <unordered_map>
#include <string>

void build_abac_pol_tree(ABAC_TREE_NODE* root, std::unordered_map<std::string, ABAC_RULE*> *abac_rules);
// void print_poltree(POLTREE tree);
// void print_pol_tree(ABAC_TREE_NODE* root, int depth = 0);
bool evaluate_aux(ABAC_TREE_NODE* root, attribute_map& ua, attribute_map& oa, int weekday, int access);

void build_abac_pol_tree(ABAC_TREE_NODE* root, std::unordered_map<std::string, ABAC_RULE*> *abac_rules) {
  std::unordered_map<std::string, std::unordered_map<std::string, int> > freq_attrib_map; // attrib_name, attrib_val, frequency

  if(abac_rules == nullptr) return;
  // if(root != nullptr) return;

  for (auto rule_hash_it = abac_rules->begin(); rule_hash_it != abac_rules->end(); rule_hash_it++) {
    attribute_map user_map = rule_hash_it->second->user_attrib_map;
    attribute_map object_map = rule_hash_it->second->object_attrib_map;

    for (auto user_map_it = user_map.begin(); user_map_it != user_map.end(); user_map_it++) {
      freq_attrib_map[user_map_it->first][user_map_it->second]++;
    }
    for (auto object_map_it = object_map.begin(); object_map_it != object_map.end(); object_map_it++) {
      freq_attrib_map[object_map_it->first][object_map_it->second]++;
    }

    if(rule_hash_it->second->weekday != -1) {
      std::string k1 = "weekday";
      std::string k2 = (rule_hash_it->second->weekday?"weekday":"weekend");
      freq_attrib_map[k1][k2] ++;
    }

    if(rule_hash_it->second->daytime != -1) {
      std::string k1 = "daytime";
      std::string k2 = (rule_hash_it->second->weekday?"day":"night");
      freq_attrib_map[k1][k2] ++;
    }
  }

  double max_entropy = -1;
  std::string attrib_name;

  // std::unordered_map<std::string, double> entropy_attrib;
  for(auto it = freq_attrib_map.begin(); it != freq_attrib_map.end(); it++) {
    double entropy = 0;
    int total = 0;

    std:: unordered_map<std::string, int> map = it->second;
    for(auto val_it = map.begin(); val_it != map.end(); val_it ++) {
      total += val_it->second;
    }

    for(auto val_it = map.begin(); val_it != map.end(); val_it ++) {
      double p_x = (double) val_it->second/total;
      if (p_x>0) entropy-=p_x*log2(p_x);
    }
    // entropy_attrib[it->first] = entropy;
    if(max_entropy < entropy) {
      max_entropy = entropy;
      attrib_name = it->first;
    }
  }

  std::unordered_map<std::string, std::unordered_map<std::string, ABAC_RULE*> > child_abac_rules; // attrib_val, rule_name, rule

  for (auto rule_hash_it = abac_rules->begin(); rule_hash_it != abac_rules->end(); rule_hash_it++) {
    attribute_map user_map = rule_hash_it->second->user_attrib_map;
    attribute_map object_map = rule_hash_it->second->object_attrib_map;

    bool check = false;

    if(user_map.count(attrib_name)) {
      check = true;
      std::string attrib_val = user_map[attrib_name];

      rule_hash_it->second->user_attrib_map.erase(attrib_name);
      child_abac_rules[attrib_val].emplace(rule_hash_it->first, rule_hash_it->second);
    }

    if(check) continue;

    if(object_map.count(attrib_name)) {
      check = true;
      std::string attrib_val = object_map[attrib_name];

      rule_hash_it->second->object_attrib_map.erase(attrib_name);
      child_abac_rules[attrib_val].emplace(rule_hash_it->first, rule_hash_it->second);
    }

    if(check) continue;

    std::string k1 = "weekday";
    std::string k2 = (rule_hash_it->second->weekday?"weekday":"weekend");

    if(attrib_name == k1 && rule_hash_it->second->weekday != -1) {
      check = true;

      rule_hash_it->second->weekday = -1;
      child_abac_rules[k2].emplace(rule_hash_it->first, rule_hash_it->second);
    }

    if(check) continue;

    k1 = "daytime";
    k2 = (rule_hash_it->second->daytime?"day":"night");

    if(attrib_name == k1 && rule_hash_it->second->daytime != -1) {
      check = true;

      rule_hash_it->second->daytime = -1;
      child_abac_rules[k2].emplace(rule_hash_it->first, rule_hash_it->second);
    }
  }

  ABAC_TREE_NODE* node = root;
  node->attrib_name = attrib_name;

  for(auto it = child_abac_rules.begin(); it != child_abac_rules.end();) {
    ABAC_TREE_NODE* child_node = new ABAC_TREE_NODE();
    node->childs.emplace(it->first, child_node);
    // bool last = true;
    // int temp_access = 0;

    for (auto rules_it = (it->second).begin(); rules_it != (it->second).end(); ) {
      if(((int)rules_it->second->user_attrib_map.size() == 0 &&  (int)rules_it->second->object_attrib_map.size() == 0 &&
            ((int)rules_it->second->weekday == -1) && ((int)rules_it->second->daytime == -1))) {
                    node->childs[(it->first)]->access |= rules_it->second->access;
                    (it->second).erase(rules_it++);
                  }
      else ++rules_it;
    }

    if((it->second).size() == 0 ) {
      child_abac_rules.erase(it++);
      continue;
    }
    else {
      build_abac_pol_tree(node->childs[it->first], &(it->second));
      ++it;
    }
   
    // if(last) child_node->access = temp_access;
    // else build_abac_pol_tree(node->childs[it->first], &(it->second));

  }

}

int ecall_make_policy(POLTREE* tree, RULE_WRAPPER* rules) {
	ABAC_TREE_NODE* root = new ABAC_TREE_NODE();
	build_abac_pol_tree(root, &(rules->abac_rules)); 
	tree->root = root;
	return RET_SUCCESS
}

bool evaluate_aux(ABAC_TREE_NODE* root, attribute_map& ua, attribute_map& oa, int weekday, int access) {
//   std::cout << root->attrib_name << std::endl;
  if(root->childs.size() == 0) {
    // at leaf node
    std::cout << (root->access) << std::endl;
    return root->access >= access;
  } else {
    std::string attrib = root->attrib_name;
    if(root->childs.count(ua[attrib])) {
    //   std::cout << "selecting " << ua[attrib] << std::endl;
      return evaluate_aux(root->childs[ua[attrib]], ua, oa, weekday, access);
    } else if(root->childs.count(oa[attrib])) {
    //   std::cout << "selecting " << oa[attrib] << std::endl;
      return evaluate_aux(root->childs[oa[attrib]], ua, oa, weekday, access);
    } else if(attrib == "weekday"){
      std::string k = weekday ? "weekday":"weekend";
    //   std::cout << "selecting " << k << std::endl;
      return evaluate_aux(root->childs[k], ua, oa, weekday, access);
    } else {
      return false;
    }
  }
}

int ecall_evaluate_aux(int* result, ABAC_TREE_NODE* root, ATTRIB_DICT_WRAPPER* ua, ATTRIB_DICT_WRAPPER* oa, int weekday, int access) {
	*result = evaluate_aux(root->root, ua->attrib_map, oa->attrib_map, weekday, access);
	return RET_SUCCESS;
}



