//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

package inet.showcases.measurement.residencetime;

import inet.networks.WiredNetworkBase;
import inet.node.ethernet.Eth100M;
import inet.node.ethernet.Eth1G;
import inet.node.ethernet.EthernetSwitch;
import inet.node.inet.StandardHost;

network ResidenceTimeMeasurementShowcase extends WiredNetworkBase
{
    submodules:
        source: StandardHost {
            @display("p=350,100");
        }
        switch: EthernetSwitch {
            @display("p=550,100");
        }
        destination: StandardHost {
            @display("p=750,100");
        }
    connections:
        source.ethg++ <--> Eth1G <--> switch.ethg++;
        switch.ethg++ <--> Eth100M <--> destination.ethg++;
}
