// @flow

import React from 'react';
import PropTypes from 'prop-types';
import { compose } from 'redux';
import { connect } from 'react-redux';

import Checkbox from './Checkbox';
import { createPoll } from '../actions';
import MainLayout from './MainLayout';

import '../css/Polls';

type PollRequest = {
  subject: string;
  options: Array<string>;
  multi_vote: boolean;
}

type PollCreateAction = (PollRequest) => void;

const OptionInput = ({
  value,
  index,
  onChange,
}: {
  value: string;
  index: number;
  onChange: (any, number) => void;
}) => (
  <div className='form-group'>
    <div className='col-sm-12'>
      <input
        className='form-control'
        type='text'
        placeholder={'Enter poll option'}
        value={value}
        onChange={event => onChange(event, index)}
      />
    </div>
  </div>
);

type PollCreateFormDefaultProps = {
  minOptionCount: number;
}

type PollCreateFormProps = PollCreateFormDefaultProps & {
  onSubmit: PollCreateAction;
}

type PollCreateFormState = {
  options: Array<string>;
}

class PollCreateForm extends React.Component<PollCreateFormProps, PollCreateFormState> {
  static defaultProps: PollCreateFormDefaultProps = {
    minOptionCount: 3,
  };

  constructor(props: PollCreateFormProps) {
    super(props);

    this.state = {
      options: new Array(this.props.minOptionCount).fill(''),
    };
  }

  handleSubmit = (event) => {
    event.preventDefault();

    const payload = {
      subject: event.target.elements.subject.value,
      options: this.state.options.filter(option => option !== ''),
      multi_vote: event.target.elements.multi_vote.checked,
    };

    if (!payload.options.length) {
      return;
    }

    this.props.onSubmit(payload);
  }

  handleOptionChange = (event, index) => {
    const options = this.state.options.slice();

    options[index] = event.target.value;

    let {length} = options;
    while (length > this.props.minOptionCount && options[-- length] === '') {
      options.pop();
    }

    this.setState({options});
  }

  render() {
    const options = [
      ...this.state.options,
      '',
    ];

    const optionInputs = options.map((value, i) => (
      <OptionInput
        key={i}
        index={i}
        value={value}
        onChange={this.handleOptionChange}
      />
    ));

    return (
      <form
        className='form-horizontal'
        onSubmit={this.handleSubmit}
      >
        <div className='form-group'>
          <div className='col-sm-12'>
            <input
              className='form-control'
              type='text'
              name='subject'
              placeholder={'Enter poll title'}
            />
          </div>
        </div>
        {optionInputs}
        <div className='form-group'>
          <div className='col-sm-12'>
            <label>
              <Checkbox
                className='form-control'
                id='poll-multi-vote'
                name='multi_vote'
              />
              <span className="poll-multi-vote-label">Allow multiple poll answers</span>
            </label>
          </div>
        </div>
        <div className='form-group'>
          <div className='col-sm-12'>
            <button
              type='submit'
              className='btn btn-primary'
              disabled={false}
            >
              Save Poll
            </button>
          </div>
        </div>
      </form>
    );
  }
}

const PollCreate = ({
  history,
  createPoll,
}: {
  history: any;
  createPoll: PollCreateAction;
}) => (
  <MainLayout history={history}>
    <div className='container'>
      <h1 className='poll-title'>Create Poll</h1>
      <PollCreateForm onSubmit={createPoll} />
    </div>
  </MainLayout>
);

function mapDispatchToProps(dispatch, props) {
  return {
    createPoll(poll) {
      return dispatch(createPoll(poll, props.history));
    },
  };
}

export default compose(
  connect(() => ({}), mapDispatchToProps),
)(PollCreate);

