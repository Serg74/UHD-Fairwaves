#!/usr/bin/env python
#
# Copyright 2011 Ettus Research LLC
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

TMPL_HEADER = """
#import time
/***********************************************************************
 * This file was generated by $file on $time.strftime("%c")
 **********************************************************************/

\#include "convert_common.hpp"
\#include <uhd/utils/byteswap.hpp>

using namespace uhd::convert;
"""

TMPL_CONV_TO_FROM_ITEM32_1 = """
DECLARE_CONVERTER(convert_$(cpu_type)_1_to_item32_1_$(swap), PRIORITY_GENERAL){
    const $(cpu_type)_t *input = reinterpret_cast<const $(cpu_type)_t *>(inputs[0]);
    item32_t *output = reinterpret_cast<item32_t *>(outputs[0]);

    for (size_t i = 0; i < nsamps; i++){
        output[i] = $(swap_fcn)($(cpu_type)_to_item32(input[i], float(scale_factor)));
    }
}

DECLARE_CONVERTER(convert_item32_1_to_$(cpu_type)_1_$(swap), PRIORITY_GENERAL){
    const item32_t *input = reinterpret_cast<const item32_t *>(inputs[0]);
    $(cpu_type)_t *output = reinterpret_cast<$(cpu_type)_t *>(outputs[0]);

    for (size_t i = 0; i < nsamps; i++){
        output[i] = item32_to_$(cpu_type)($(swap_fcn)(input[i]), float(scale_factor));
    }
}
"""
TMPL_CONV_TO_FROM_ITEM32_X = """
DECLARE_CONVERTER(convert_$(cpu_type)_$(width)_to_item32_1_$(swap), PRIORITY_GENERAL){
    #for $w in range($width)
    const $(cpu_type)_t *input$(w) = reinterpret_cast<const $(cpu_type)_t *>(inputs[$(w)]);
    #end for
    item32_t *output = reinterpret_cast<item32_t *>(outputs[0]);

    for (size_t i = 0, j = 0; i < nsamps; i++){
        #for $w in range($width)
        output[j++] = $(swap_fcn)($(cpu_type)_to_item32(input$(w)[i], float(scale_factor)));
        #end for
    }
}

DECLARE_CONVERTER(convert_item32_1_to_$(cpu_type)_$(width)_$(swap), PRIORITY_GENERAL){
    const item32_t *input = reinterpret_cast<const item32_t *>(inputs[0]);
    #for $w in range($width)
    $(cpu_type)_t *output$(w) = reinterpret_cast<$(cpu_type)_t *>(outputs[$(w)]);
    #end for

    for (size_t i = 0, j = 0; i < nsamps; i++){
        #for $w in range($width)
        output$(w)[i] = item32_to_$(cpu_type)($(swap_fcn)(input[j++]), float(scale_factor));
        #end for
    }
}
"""

def parse_tmpl(_tmpl_text, **kwargs):
    from Cheetah.Template import Template
    return str(Template(_tmpl_text, kwargs))

if __name__ == '__main__':
    import sys, os
    file = os.path.basename(__file__)
    output = parse_tmpl(TMPL_HEADER, file=file)
    for width in 1, 2, 3, 4:
        for swap, swap_fcn in (('nswap', ''), ('bswap', 'uhd::byteswap')):
            for cpu_type in 'fc64', 'fc32', 'sc16':
                output += parse_tmpl(
                    TMPL_CONV_TO_FROM_ITEM32_1 if width == 1 else TMPL_CONV_TO_FROM_ITEM32_X,
                    width=width, swap=swap, swap_fcn=swap_fcn, cpu_type=cpu_type
                )
    open(sys.argv[1], 'w').write(output)
