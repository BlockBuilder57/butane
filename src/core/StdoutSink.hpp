#pragma once

#include <core/Logger.hpp>

namespace butane::core {

	/// A logger sink implementation that prints to standard output.
	struct StdoutSink : public Logger::Sink {
		static StdoutSink& The();

		virtual void OutputMessage(const Logger::MessageData& data) override;
	};

	/// Attach the stdout logger sink to the global logger.
	void LoggerAttachStdout();

} // namespace lucore
