import React, { Component } from 'react';
import { Scrollbars } from 'react-custom-scrollbars';

export default class ColoredScrollbars extends Component {

    constructor(...rest) {
        super( ...rest);
        this.renderView = this.renderView.bind(this);
        this.renderThumb = this.renderThumb.bind(this);
    }

    renderView({ style }) {
        const viewStyle = {
            padding: 10,
        };
        return (
            <div
                className="box"
                style={{ ...style, ...viewStyle }}
                />
        );
    }

    renderThumb({ style }) {
        const thumbStyle = {
            backgroundColor: "rgb(55, 55, 55)"
        };
        return (
            <div
                style={{ ...style, ...thumbStyle }}
               />
        );
    }

    render() {
        return (
            <Scrollbars
                renderView={this.renderView}
                renderThumbVertical={this.renderThumb}
                {...this.props}/>
        );
    }
}