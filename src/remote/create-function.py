#!/usr/bin/env python3

import os
import sys
from zipfile import ZipFile
import tempfile
import shutil
import argparse
import hashlib
import base64
import boto3

import json

BASE_FILE = {
    "lambda": "lambda_function/packages.zip",
    "meow": "meow_function/packages.zip"
}

PACKAGE_GG_DIR = "_gg"

functions = [
    ("lambda", []), # the generic function
    ("meow", []),

    #["gcc", "cc1"],
    #["gcc", "as"],
    #["gcc", "collect2", "ld"],

    #["g++", "cc1plus"],
    #["g++", "as"],
    #["g++", "collect2", "ld"],

    #["ranlib"],
    #["ar"],
    #["strip"],
    #["ld"],
]

hash_cache = {}

def executable_hash(hashes):
    hashes.sort()
    str_to_hash = "".join(hashes).encode('ascii')
    return "{}".format(base64.urlsafe_b64encode(hashlib.sha256(str_to_hash).digest()).decode('ascii').replace('=',''))


def gghash(filename, block_size=65536):
    sha256 = hashlib.sha256()
    size = 0

    with open(filename, 'rb') as f:
        for block in iter(lambda: f.read(block_size), b''):
            size += len(block)
            sha256.update(block)

    return "V{}{:08x}".format(base64.urlsafe_b64encode(sha256.digest()).decode('ascii').replace('=','').replace('-', '.'), size)

def create_function_package(label, output, function_execs, gg_execute_static, gg_meow_worker):
    PACKAGE_FILES = {
        "gg-execute-static": gg_execute_static,
        "main.py": "%s_function/main.py" % label,
        "ggpaths.py": "common/ggpaths.py",
        "common.py": "common/common.py"
    }

    if label == 'meow':
        PACKAGE_FILES['gg-meow-worker'] = gg_meow_worker

    for exe in function_execs:
        PACKAGE_FILES["executables/{}".format(exe[0])] = exe[1]

    shutil.copy(BASE_FILE[label], output)

    with ZipFile(output, 'a') as funczip:
        for fn, fp in PACKAGE_FILES.items():
            funczip.write(fp, fn)

def get_vpc_config(region):
    client = boto3.client('ec2', region)
    try:
        # vpcs_dict = client.describe_vpcs()
        # print(json.dumps(vpcs_dict, indent=4, sort_keys=True))

        subnets_dict = client.describe_subnets()
        # print(json.dumps(subnets_dict, indent=4, sort_keys=True))

        sec_groups_dict = client.describe_security_groups()
        # print(json.dumps(sec_groups_dict, indent=4, sort_keys=True))
    except:
        print("get_vpc_config with exception")
        vpc_cfg_dict = {}
        return vpc_cfg_dict

    # vpc_ids = []
    subnet_ids = []
    sec_group_ids = []

    #for Vpc in vpcs_dict["Vpcs"]:
    #    VpcId = Vpc["VpcId"]
    #    vpc_ids.append(VpcId)

    for Subnet in subnets_dict["Subnets"]:
        SubnetId = Subnet["SubnetId"]
        subnet_ids.append(SubnetId)

    for SecurityGroup in sec_groups_dict["SecurityGroups"]:
        GroupId = SecurityGroup["GroupId"]
        sec_group_ids.append(GroupId)


    print("SubnetIds -> %s", subnet_ids)
    print("SecurityGroups -> %s", sec_group_ids)

    vpc_cfg_dict = {'SubnetIds' : subnet_ids, 'SecurityGroupIds' : sec_group_ids}
    # print(json.dumps(vpc_cfg_dict, indent=4, sort_keys=True))
    # vpc_cfg_str = json.dumps(vpc_cfg_dict, indent=4, sort_keys=True)
    # print("vpc_cfg_str -> %s" % vpc_cfg_str)

    return vpc_cfg_dict

def install_lambda_package(package_file, function_name, role, region, delete=False):
    with open(package_file, 'rb') as pfin:
        package_data = pfin.read()

    client = boto3.client('lambda', region_name=region)

    if delete:
        try:
            client.delete_function(FunctionName=function_name)
        except:
            pass

    vpc_cfg_dict = get_vpc_config(region)

    response = client.create_function(
        FunctionName=function_name,
        Runtime='python3.6',
        Role=role,
        VpcConfig=vpc_cfg_dict,
        Handler='main.handler',
        Code={
            'ZipFile': package_data
        },
        Timeout=300,
        MemorySize=3008,
        Tags={
            'gg': 'generic',
        }
    )

    print(response['FunctionArn'])

def main():
    parser = argparse.ArgumentParser(description="Generate and install Lambda functions.")
    parser.add_argument('--delete', dest='delete', action='store_true', default=False)
    parser.add_argument('--role', dest='role', action='store',
                        default=os.environ.get('GG_LAMBDA_ROLE'))
    parser.add_argument('--region', dest='region', default=os.environ.get('AWS_REGION'), action='store')
    parser.add_argument('--gg-execute-static', dest='gg_execute_static',
                        default=shutil.which("gg-execute-static"))
    parser.add_argument('--gg-meow-worker', dest='gg_meow_worker',
                        default=shutil.which("gg-meow-worker"))
    parser.add_argument('--toolchain-path', dest='toolchain_path', required=True)

    args = parser.parse_args()

    if not args.gg_execute_static:
        raise Exception("Cannot find gg-execute-static")

    if not args.role:
        raise Exception("Please provide function role (or set GG_LAMBDA_ROLE).")

    for label, func in functions:
        function_execs = [(f, os.path.join(args.toolchain_path, f)) for f in func]
        function_execs = [(gghash(f[1]), f[1]) for f in function_execs]
        hashes = [f[0] for f in function_execs]

        if len(function_execs) == 0:
            function_name = "gg-%s-function" % label
        else:
            function_name = "{prefix}{exechash}".format(
                prefix="gg-", exechash=executable_hash(hashes)
            )

        function_file = "{}.zip".format(function_name)
        create_function_package(label, function_file, function_execs, args.gg_execute_static, args.gg_meow_worker)
        print("Installing lambda function {}... ".format(function_name), end='')
        install_lambda_package(function_file, function_name, args.role, args.region,
                               delete=args.delete)
        print("done.")
        os.remove(function_file)

if __name__ == '__main__':
    main()
