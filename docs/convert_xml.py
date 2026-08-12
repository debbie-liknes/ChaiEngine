#!/usr/bin/python env

# Currently not in use, but checking it in for now. This was a scrapped custom XML converter from doxygen. At the moment
# things are back to using Breathe out-of-the-box until we need something better.

import xml.etree.ElementTree as ET
from pathlib import Path
import re
import os

excluded_classes = [
    "AssetCacheTest",
    "component_count",
    "is_vec"
]

dir_path = Path('./doxygen/xml')
os.makedirs('classes', exist_ok=True)
file_list = list()
for kw in ('class', 'struct'):
    file_list.extend(dir_path.glob(f'{kw}*.xml'))

for file in file_list:
    print(f'Processing {file}')
    
    tree = ET.parse(file)
    root = tree.getroot()
    
    compound_def = root.find('compounddef')
    
    compound_name = compound_def.find('compoundname').text
    
    # We don't do templated things...yet
    if compound_name.find('<') != -1:
        continue
    
    class_name = compound_name.split('::')[-1]
    
    if class_name in excluded_classes:
        continue
    
    # TODO: Godot's XML-->RST conversion does not support multi-inheritance
    # Once replaced, we need to change this.
    base_compound_ref = compound_def.find('basecompoundref')
    inherits = ''
    if base_compound_ref is not None:
        # TODO: Godot's XML-->RST conversion does not support templates
        # Once replaced, we need to change this.
        inherits = re.sub('<.*>', '', base_compound_ref.text.split('::')[-1])
    
    brief_description = ''.join(compound_def.find('briefdescription').itertext())
    detailed_description = ''.join(compound_def.find('detaileddescription').itertext())
    
    section_defs = compound_def.findall('sectiondef')
    methods = []
    for section_def in section_defs:
        if section_def.get('kind') == 'public-func':
            for member_def in section_def.findall('memberdef'):
                methods.append({
                    'name': ''.join(member_def.find('name').itertext()),
                    'type': ''.join(member_def.find('type').itertext()),
                    'qualifiers': '',
                    'description': '',
                })
    
    out_class = ET.Element('class', name=class_name, inherits=inherits, api_type='core', version='1.0')
    out_brief_description = ET.SubElement(out_class, 'brief_description')
    out_brief_description.text = brief_description
    out_description = ET.SubElement(out_class, 'description')
    out_description = detailed_description
    # out_tutorials = ET.SubElement(out_class, 'tutorials')
    out_constructors = ET.SubElement(out_class, 'constructors')
    out_methods = ET.SubElement(out_class, 'methods')
    for method in methods:
        out_method = ET.SubElement(out_methods, 'method', name=method['name'], qualifiers=method['qualifiers'])
        out_return = ET.SubElement(out_method, 'return', type=method['type'])
        out_description = ET.SubElement(out_method, 'description')
        out_description.text = method['description']
    # out_members = ET.SubElement(out_class, 'members')
    # out_signals = ET.SubElement(out_class, 'signals')
    # out_constants = ET.SubElement(out_class, 'constants')
    # out_annotations = ET.SubElement(out_class, 'annotations')
    # out_theme_items = ET.SubElement(out_class, 'theme_items')
    # out_operators = ET.SubElement(out_class, 'operators')
    
    out_tree = ET.ElementTree(out_class)
    ET.indent(out_tree, space="    ", level=0)
    
    with open(f'classes/{class_name}.xml', 'wb') as files:
        out_tree.write(files, encoding='utf-8', xml_declaration=True)
