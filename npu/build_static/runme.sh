rm *.csv
#
# simple-npu: Example NPU simulation model using the PFPSim Framework
#
# Copyright (C) 2016 Concordia Univ., Montreal
#     Samar Abdi
#     Umair Aftab
#     Gordon Bailey
#     Faras Dewal
#     Shafigh Parsazad
#     Eric Tremblay
#
# Copyright (C) 2016 Ericsson
#     Bochra Boughzala
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#

if [ "$#" -ne 3 ]; then
    echo "Invalid Args passed"
    echo "Usage: $0 <verbosity level> <application: [nat, router, ecmp]> <debug_level: [none,debug]"
    exit
fi
vlvl="$1"
application="$2"
debug="$3"

#echo $vlvl
#./npu-sim -c Configs/ -Xp4 simple_router.json -Xtpop table.txt -Xin Configs/input.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl" | sed "s,\x1B\[[0-9;]*[a-zA-Z],,g"
#./npu-sim -c Configs/ -Xp4 simple_router.json -Xtpop table-new.txt -Xin Configs/10kflows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl" | sed "s,\x1B\[[0-9;]*[a-zA-Z],,g"
#./npu-sim -c Configs/ -Xp4 simple_router.json -Xtpop router_table.txt -Xin Configs/10kflows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl" | sed "s,\x1B\[[0-9;]*[a-zA-Z],,g"

if [[ $debug == "debug" ]] ; then
    if [[ $application == "nat" ]] ; then #NAT
        pfpdb npu-sim --args "-Xp4 apps/nat/nat.json -Xtpop apps/nat/nat_table.txt -v "$vlvl" -Xin apps/pcaps/10kflows.pcap -Xvalidation-out output.pcap" -v
    elif [[ $application == "ecmp" ]]; then
        pfpdb npu-sim --args "-Xp4 apps/ecmp/ecmp.json -Xtpop apps/ecmp/ecmp_table.txt -v "$vlvl" -Xin apps/pcaps/10kflows.pcap -Xvalidation-out output.pcap" -v
    elif [[ $application == "router" ]]; then
        pfpdb npu-sim --args "-Xp4 simple_router.json -Xtpop apps/router/router_table.txt -v "$vlvl" -Xin apps/pcaps/170pkts.pcap -Xvalidation-out output.pcap" -v
    else
        echo "not yet implemented.."
        exit
    fi

else
    if [[ $application == "nat" ]] ; then #NAT
        ./npu-sim -c Configs/ -Xp4 apps/nat/nat.json -Xtpop apps/nat/nat_table.txt -Xin apps/pcaps/10kflows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl" | sed "s,\x1B\[[0-9;]*[a-zA-Z],,g"
    elif [[ $application == "ecmp" ]]; then
        ./npu-sim -c Configs/ -Xp4 apps/ecmp/ecmp.json -Xtpop apps/ecmp/ecmp_table.txt -Xin apps/pcaps/10kflows.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl" | sed "s,\x1B\[[0-9;]*[a-zA-Z],,g"
    elif [[ $application == "router" ]]; then
        ./npu-sim -c Configs/ -Xp4 simple_router.json -Xtpop apps/router/router_table.txt -Xin apps/pcaps/170pkts.pcap -Xvalidation-out reordered-output.pcap -v "$vlvl" | sed "s,\x1B\[[0-9;]*[a-zA-Z],,g"

    else
        echo "not yet implemented.."
        exit
    fi

fi