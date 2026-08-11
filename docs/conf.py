# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'Chai Engine'
copyright = '2026, Chai'
author = 'Chai'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx.ext.autodoc',
    'breathe',
]

templates_path = ['_templates']
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

breathe_projects = {"chai": "./doxygen/xml/"}

breathe_default_project = "chai"

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

#html_theme = 'alabaster'
html_static_path = ['_static']
html_favicon = '_static/chai-cup.svg'
html_logo = '_static/chai-cup.svg'


import subprocess

# Automatically build Doxygen XML on Sphinx execution
subprocess.call('doxygen Doxyfile', shell=True)


html_theme = 'sphinx_rtd_theme'

html_theme_options = {
}

html_css_files = [
    'css/custom.css',
]
