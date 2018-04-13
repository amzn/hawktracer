export class Protocol
{
    registerAddKlassHandler(handler) {
        this._addKlassHandler = handler;
        return this;
    }

    registerAddFieldHandler(handler) {
        this._addFieldHandler = handler;
        return this;
    }

    registerDataHandler(handler) {
        this._dataHandler = handler;
        return this;
    }

    registerUpdateTSRangeHandler(handler) {
        this._updateTSRangeHandler = handler;
        return this;
    }

    registerErrorHandler(handler) {
        this._errorHandler = handler;
        return this;
    }

    registerGraphTypesHandler(handler) {
        this._graphTypesHandler = handler;
        return this;
    }

    registerSetGraphPropertiesHandler(handler) {
        this._setGraphProperties = handler;
        return this;
    }

    handleMessage(jsonMessage) {
        let object = JSON.parse(jsonMessage);
        switch (object.command) {
            case "addKlass":
                this._callHandler(this._addKlassHandler, object);
                break;
            case "addField":
                this._callHandler(this._addFieldHandler, object);
                break;
            case "data":
                this._callHandler(this._dataHandler, object);
                break;
            case "totalTSRange":
                this._callHandler(this._updateTSRangeHandler, object);
                break;
            case "error":
                this._callHandler(this._errorHandler, object);
                break;
            case "graphTypes":
                this._callHandler(this._graphTypesHandler, object);
                break;
            case "setGraphProperties":
                this._callHandler(this._setGraphProperties, object);
                break;
        }
    }

    _callHandler(handler, obj) {
        if (handler) {
            handler(obj);
        }
    }

    makeSetCurrentTSRangeRequest(duration, stopTimestamp) {
        return JSON.stringify({
            command: "setCurrentTSRange",
            duration: duration,
            stopTimestamp: stopTimestamp
        });
    }

    makeTrackFieldRequest(klassId, fieldName) {
        return JSON.stringify({
            command: "track_field",
            klassId: klassId,
            fieldName: fieldName
        });
    }

    makeGetTotalTSRangeRequest() {
        return '{"command": "getTotalTSRange"}';
    }

    makeGetDataRequest() {
        return '{"command": "getData"}';
    }

    makeNewGraphRequest(graphId, graphTypeId, graphDescription) {
        return JSON.stringify({
            command: "createGraph",
            graphTypeId: graphTypeId,
            graphId: graphId,
            graphDescription: graphDescription
        });
    }
}