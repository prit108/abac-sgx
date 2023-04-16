# from poltree import *
import json

def get_code(a):
    if(a == "read"): return 1
    elif(a == "write"): return 2
    else: return 0

def policy_rules_parse(policy_file):
    file = open(policy_file, "r")
    rules_json = json.load(file)
    file.close()
    print(len(rules_json))
    # r1 = ABAC_RULE("r1", 0, , rules_json[0]["ua"], rules_json[0]["oa"])
    # print(r1.get_user_attribute_value("designation"))
    rule_list = []
    for rule in rules_json:
        rule_list.append(
            ABAC_RULE(
                rule["name"],
                get_code(rule["op"]),
                int(rule["ea"]["day"] == "weekday"),
                rule["ua"],
                rule["oa"]
            )
        )

    return rule_list

def build(policy_file):
    rules = policy_rules_parse(policy_file)
    poltree = build_policy(rules)
    return poltree

def evalute_request(poltree, ua, oa, access):
    result = evaluate(poltree, ua, oa, access)
    return result