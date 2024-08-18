#include <arte.h>

using namespace arte;

int main(int argc, char** argv)
{
	const short port = 55555;
	std::puts("arte started");

	int returnCode = 0;

	if (!python::API::Init()) {
		returnCode = -1;
		std::puts(std::format("arte cannot init python, exiting with code {}",
							  returnCode)
					  .c_str());
		return returnCode;
	}

	python::API::LoadScript("scripts/read_types.py");

	python::API::LoadScript("scripts/transactions/gen_rand.py");
	python::API::LoadScript("scripts/transactions/gen_pitch.py");
	python::API::LoadScript("scripts/transactions/gen_major_scale.py");
	python::API::LoadScript("scripts/transactions/gen_major_chord.py");

	python::API::LoadScript("scripts/demo.py");
	python::API::OnCall(8, NULL);

	try {
		asio::io_context ioContext(1);
		asio::signal_set signals(ioContext, SIGINT, SIGTERM);
		signals.async_wait([&](auto, auto) { ioContext.stop(); });
		std::puts("asio::io_context initialized");

		Server server(ioContext, port);

		// will block untill all tasks finished
		ioContext.run();
		server.Close();
	}
	catch (std::exception& e) {
		std::puts(std::format("{}", e.what()).c_str());
		python::API::Close();
		returnCode = -1;
		std::puts(std::format("arte ended with code {}", returnCode).c_str());
		return returnCode;
	}

	python::API::Close();
	std::puts(std::format("arte ended with code {}", returnCode).c_str());
	return returnCode;
}