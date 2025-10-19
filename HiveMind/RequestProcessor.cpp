//
// Created by vlad on 10/18/25.
//

#include "RequestProcessor.h"

HiveCommand RequestProcessor::toCommand(const HttpRequest& req) {
    if (auto command = tryCreateAddInterference(req)) {
        return *command;
    }
    if (auto command = tryCreateRemoveInterference(req)) {
        return *command;
    }
    if (auto command = tryCreateStop(req)) {
        return *command;
    }
    if (auto command = tryCreateMove(req)) {
        return *command;
    }
    return {HiveCommand::NotFound, req.clientFd};
}

std::optional<HiveCommand> RequestProcessor::tryCreateAddInterference(const HttpRequest &req) {
    if (!isPost(req)) {
        return {};
    }
    if (!isCommand(req)) {
        return {};
    }

    int commandTypeValue = req.body.value("CommandType", -1);
    if (commandTypeValue != 4) {
        return {};
    }

    HiveCommand command;
    command.type = HiveCommand::AddInterference;
    command.clientFd = req.clientFd;

    if (!req.body.contains("Interference")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }
    nlohmann::json interference = req.body.at("Interference");

    if (!interference.contains("Id")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }

    if (!interference.contains("Location")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }

    if (!interference.contains("RadiusKM")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }

    nlohmann::json location = interference.at("Location");
    if (!location.contains("Latitude")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }
    if (!location.contains("Longitude")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }

    command.arguments["Id"] = interference["Id"];
    command.arguments["RadiusKM"] = interference["RadiusKM"];
    command.arguments["Location"] = location;
    return command;
}

std::optional<HiveCommand> RequestProcessor::tryCreateRemoveInterference(const HttpRequest &req) {
    if (!isPost(req)) {
        return {};
    }
    if (!isCommand(req)) {
        return {};
    }

    int commandTypeValue = req.body.value("CommandType", -1);
    if (commandTypeValue != 5) {
        return {};
    }

    HiveCommand command;
    command.type = HiveCommand::RemoveInterference;
    command.clientFd = req.clientFd;

    if (!req.body.contains("InterferenceId")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }

    command.arguments["Id"] = req.body["InterferenceId"];
    return command;
}

std::optional<HiveCommand> RequestProcessor::tryCreateStop(const HttpRequest &req) {
    if (!isPost(req)) {
        return {};
    }
    if (!isCommand(req)) {
        return {};
    }

    int commandTypeValue = req.body.value("CommandType", -1);
    if (commandTypeValue != 1) {
        return {};
    }

    HiveCommand command;
    command.type = HiveCommand::Stop;
    command.clientFd = req.clientFd;
    return command;
}

std::optional<HiveCommand> RequestProcessor::tryCreateMove(const HttpRequest &req) {
    if (!isPost(req)) {
        return {};
    }
    if (!isCommand(req)) {
        return {};
    }

    int commandTypeValue = req.body.value("CommandType", -1);
    if (commandTypeValue != 2) {
        return {};
    }

    HiveCommand command;
    command.type = HiveCommand::Move;
    command.clientFd = req.clientFd;

    if (!req.body.contains("Destination")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }
    nlohmann::json destination = req.body.at("Destination");
    if (!destination.contains("Latitude")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }
    if (!destination.contains("Longitude")) {
        command.type = HiveCommand::WrongArgument;
        return command;
    }

    command.arguments["Latitude"] = destination["Latitude"];
    command.arguments["Longitude"] = destination["Longitude"];
    return command;
}

bool RequestProcessor::isPost(const HttpRequest &req) {
    return req.method == "POST";
}

bool RequestProcessor::isCommand(const HttpRequest &req) {
    if (req.path != "/api/v1/command") {
        return false;
    }
    return req.body.contains("CommandType");
}
