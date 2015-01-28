from migen.fhdl.std import *
from migen.bus import wishbone
from migen.bus.transactions import *
from migen.sim.generic import run_simulation

from liteeth.common import *
from liteeth.mac import LiteEthMAC

from liteeth.test.common import *
from liteeth.test.model import phy, mac

class TB(Module):
	def __init__(self):
		self.submodules.hostphy = phy.PHY(8, debug=True)
		self.submodules.hostmac = mac.MAC(self.hostphy, debug=True, random_level=0)
		self.submodules.ethmac = LiteEthMAC(phy=self.hostphy, dw=32, interface="core", with_hw_preamble_crc=True)

		self.submodules.streamer = PacketStreamer(eth_mac_description(32), last_be=1)
		self.submodules.streamer_randomizer = AckRandomizer(eth_mac_description(32), level=0)

		self.submodules.logger_randomizer = AckRandomizer(eth_mac_description(32), level=0)
		self.submodules.logger = PacketLogger(eth_mac_description(32))

		# use sys_clk for each clock_domain
		self.clock_domains.cd_eth_rx = ClockDomain()
		self.clock_domains.cd_eth_tx = ClockDomain()
		self.comb += [
			self.cd_eth_rx.clk.eq(ClockSignal()),
			self.cd_eth_rx.rst.eq(ResetSignal()),
			self.cd_eth_tx.clk.eq(ClockSignal()),
			self.cd_eth_tx.rst.eq(ResetSignal()),
		]

		self.comb += [
			Record.connect(self.streamer.source, self.streamer_randomizer.sink),
			Record.connect(self.streamer_randomizer.source, self.ethmac.sink),
			Record.connect(self.ethmac.source, self.logger_randomizer.sink),
			Record.connect(self.logger_randomizer.source, self.logger.sink)
		]

	def gen_simulation(self, selfp):
		selfp.cd_eth_rx.rst = 1
		selfp.cd_eth_tx.rst = 1
		yield
		selfp.cd_eth_rx.rst = 0
		selfp.cd_eth_tx.rst = 0

		for i in range(8):
			streamer_packet = Packet([i for i in range(64)])
			print(streamer_packet)
			yield from self.streamer.send(streamer_packet)

if __name__ == "__main__":
	run_simulation(TB(), ncycles=1000, vcd_name="my.vcd", keep_files=True)
