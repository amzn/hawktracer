import React from 'react';

export class KlassFieldSelector extends React.Component {
    activeKlass = undefined

    constructor(props) {
        super(props);
        this.state = {
            fields: []
        };

        this.klassChanged = this.klassChanged.bind(this);
        this.fieldChanged = this.fieldChanged.bind(this);
    }

    notifyChange() {
        if (this.props.onKlassFieldChanged) {
            this.props.onKlassFieldChanged(this.activeKlass, this.activeField, this.props.name);
        }
    }

    klassChanged(event) {
        const klassName = event.target.value;
        this.activeKlass = this.props.klasses.find((v) => v.name == klassName);
        this.activeField = this.activeKlass.fields[0];
        this._fieldSelector.selectedIndex = 0;

        this.setState({
            fields: this.activeKlass.fields
        });

        this.notifyChange();
    }

    fieldChanged(event) {
        const fieldName = event.target.value;
        this.activeField = this.activeKlass.fields.find((v) => v.name == fieldName);

        this.notifyChange();
    }

    componentWillReceiveProps(nextProps) {
        if (this.activeKlass == undefined) {
            this.activeKlass = nextProps.klasses[0];
        }

        if (this.activeKlass != undefined && this.activeField == undefined) {
            let klass = nextProps.klasses.find(element => element.name == this.activeKlass.name);
            if (klass == undefined) {
                return;
            }
            if (klass.fields.length != this.state.fields.length) {
                this.setState({fields: klass.fields});
            }
        }
    }

    render() {
        return (
            <div>
                <select onChange={this.klassChanged}>
                    {this.props.klasses.map((value) => <option value={value.name}>{value.name}</option>)}
                </select>
                <select ref={fieldSelector => this._fieldSelector = fieldSelector} onChange={this.fieldChanged}>
                    {this.state.fields.map((field) => <option value={field.name}>{field.name} ({field.type})</option>)}
                </select>
            </div>)
    }
}

KlassFieldSelector.defaultProps = {
    klasses: [],
    onKlassFieldChanged: undefined
};